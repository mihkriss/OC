#ifndef MLTSHR
#define MLTSHR
#define MLTSHR_BLOCK_SIZE 4096

#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <stdlib.h>
 
typedef struct mltshr
{
	char memname[20]; // максимальная длина имени пайпа
	int memFd;  // создание переменной файлого дисриптора
	char* buffer;
} mltshr;

typedef struct state
{
	pthread_mutex_t mutex; // защищает отдельно, чтобы несколько потокоы не читали одновременно
	pthread_mutex_t write_mutex; // защащает отдельно, чтобы несколько потоков не писали одновременно
	pthread_cond_t cond; //условная переменная,для синхранихации между потоками
	int msglen; // кол-во байт, которое нужно отобразить в память
} state;

int mltshr_create(mltshr *ms, char *memname, char host)
{
	ms->memFd = shm_open(memname, O_CREAT | O_RDWR, S_IWUSR | S_IRUSR); // определяем обственно создаваемый объект разделяемой памяти для создания или открытия
	if(ms->memFd == -1) // в случае успеха: возврат неотрицательного дескриптора
		return -1;
	if(host)
		if(ftruncate(ms->memFd, MLTSHR_BLOCK_SIZE+sizeof(state))) // обрезка файла, определяемого fd , до указанного размера в байтах
			return -1;
	ms->buffer = mmap(NULL, MLTSHR_BLOCK_SIZE+sizeof(state), PROT_READ | PROT_WRITE, MAP_SHARED, ms->memFd, 0); // разделение использования отображения с другими процессами
	if(ms->buffer == (void*)-1)
		return -1;
	if(host)
	{
		pthread_mutexattr_t attrmutex;
		pthread_condattr_t attrcond;
		if(pthread_mutexattr_init(&attrmutex) ||                      // инициализация мьютекс
			pthread_mutexattr_setpshared(&attrmutex, PTHREAD_PROCESS_SHARED) ||  // позволить использовать мьютекс для синхронизации потоков в разных процессах.
			pthread_mutex_init(&(((state*)(ms->buffer))->mutex), &attrmutex) ||
			pthread_mutex_init(&(((state*)(ms->buffer))->write_mutex), &attrmutex) ||
			pthread_mutex_lock(&(((state*)(ms->buffer))->write_mutex)) ||
			pthread_condattr_init(&attrcond) ||                        // инициализация кондов
			pthread_condattr_setpshared(&attrcond, PTHREAD_PROCESS_SHARED) ||
			pthread_cond_init(&(((state*)(ms->buffer))->cond), &attrcond)) 
		return -1;
	}
	memcpy(ms->memname, memname, strlen(memname) + 1); // копируем кол-во байт из участка памяти на который дейсвует указатель, и +1 к кол-во символов
	return 0;
}

void mltshr_destroy(mltshr *ms)
{
	pthread_mutex_destroy(&(((state*)(ms->buffer))->mutex));  // После использования мьютекса его необходимо уничтожить
	pthread_mutex_destroy(&(((state*)(ms->buffer))->write_mutex));
	pthread_cond_destroy(&(((state*)(ms->buffer))->cond));  // используется для удаления переменной
	munmap(ms->buffer, MLTSHR_BLOCK_SIZE); // отображает длину в байтах 
	shm_unlink(ms->memname); // удаляется имя объекта разделяемой памяти и, как только все процессы завершили работу с объектом и отменили его распределение, очищают пространство и уничтожают связанную с ним область памяти.
	close(ms->memFd);
	ms->memFd = -1;
}

void mltshr_write(mltshr *ms, char *msg, int mlen)
{
	pthread_mutex_lock(&(((state*)(ms->buffer))->write_mutex));
	pthread_mutex_lock(&(((state*)(ms->buffer))->mutex));
	((state*)(ms->buffer))->msglen = mlen;
	memcpy(ms->buffer+sizeof(state), msg, mlen);
	pthread_cond_broadcast(&(((state*)(ms->buffer))->cond)); // разблокировать все потоки, заблокированные в данный момент для указанной переменной условия cond 
    pthread_mutex_unlock(&(((state*)(ms->buffer))->mutex));  // , заблокировавший мьютекс
}

char* mltshr_read(mltshr *ms, int *len)
{
	pthread_mutex_lock(&(((state*)(ms->buffer))->mutex)); // блокировка мьтекса
	pthread_mutex_unlock(&(((state*)(ms->buffer))->write_mutex)); // разюлокировка
	pthread_cond_wait(&(((state*)(ms->buffer))->cond), &(((state*)(ms->buffer))->mutex)); // возвращает запертый мьютекс, который принадлежит вызывающему потоку, даже если возникла ошибка. .
	int mlen = (((state*)(ms->buffer))->msglen);
	*len = mlen;
	char *mem = malloc(mlen);
	memcpy(mem, ms->buffer+sizeof(state), mlen);
	pthread_mutex_unlock(&(((state*)(ms->buffer))->mutex));
	return mem;
}

#endif

