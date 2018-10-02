#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


#include "cthread.h"
#include "support.h"
#include "cdata.h"

/* Funções auxiliares */
TCB_t* searchTID(PFILA2 pFila, int tid);

/* Variáveis Globais */
int tid = 0;

/* Filas */
FILA2 aptoAltaPrior;
PFILA2 APTO_ALTA = &aptoAltaPrior;
FILA2 aptoMediaPrior;
PFILA2 APTO_MEDIA = &aptoMediaPrior;
FILA2 aptoBaixaPrior;
PFILA2 APTO_BAIXA = &aptoBaixaPrior;
FILA2 filaBloqueado;
PFILA2 BLOQUEADO = &filaBloqueado;
FILA2 filaExecutando;
PFILA2 F_EXECUTANDO = &filaExecutando;
FILA2 filaJoin;
PFILA2 JOIN = &filaJoin;


/* Ponteiro para o TCB em execução */
TCB_t *EXECUTANDO;

/* Variável u_context para facilitar o retorno */
ucontext_t r_context;


int ccreate (void *(*start) (void *), void *arg, int prio)
{
	if(tid ==0){
		//caso em que as filas falharam em sua criação
		if(CreateFila2(APTO_ALTA)) return -1;
		if(CreateFila2(APTO_MEDIA)) return -1;
		if(CreateFila2(APTO_BAIXA)) return -1;
		if(CreateFila2(APTO_BLOQUEADO)) return -1;
		if(CreateFila2(APTO_F_EXECUTANDO)) return -1;

		TCB_t* new_thread = (TCB_t*) malloc(sizeof(TCB_t));
		
		// Fazer tratamento caso o tid seja 0, que é a thread main;
		/* Nesse caso, criar as filas, alocar um tcb para main, colocá-la no executando
		"Para a criação desse contexto devem  ser utilizadas as mesmas chamadas
		getcontext() e makecontext(), usadas na criação de threads com a ccreate."
		*/

		new_thread->prio = prio; // prioridade da thread é passada no parâmetro
		new_thread->tid = tid;
		new_thread->state = PROCST_APTO; // a thread está apta
		getcontext(&(new_thread->context));

		EXECUTANDO = new_thread;
		

		/* Making thread context */
		getcontext(&new_thread->context);
		new_thread->context.uc_stack.ss_sp = (char*) malloc(SIGSTKSZ);
		new_thread->context.uc_stack.ss_size = SIGSTKSZ;
		new_thread->context.uc_link = &control.ended_thread;
		makecontext(&new_thread->context, (void (*)(void))start, 1, arg);


		/* Put it into all_treads and able_threads */
		if (!rb_insert(control.all_threads, new_thread->tid, new_thread))
			return FALSE;
		if (!rb_able_insert(new_thread->tid))
			return FALSE;
	}
	

	return new_thread->tid;
}

// Cedência voluntária:
int cyield(void){
	EXECUTANDO->state = PROCST_APTO; //Passa de executando pra apto.
	if(inserirApto(EXEC)) // retorna 0 caso tenha obtido sucesso, igual ao AppendFila2
		return -1;
	else{
		// ESCALONADOR
		return 0;
	}
}

// Setar prioridade:
int csetprio(int tid, int prio){
		//Aparentemente é pra deixar tid em null em 2018/2, então o csetprio funciona apenas na thread corrente.
		if(prio >= 0 && prio <= 2){
			if(EXECUTANDO->prio > prio){
			// Executar caso o processo tenha baixado a prioridade, executar o escalonador
			return 0;
			}
			else{
				EXECUTANDO->prio = prio;
				return 0;
			}
		}
		else //valor de prioridade é inválido, deve estar entre [0,2].
			return -1;
}

int cjoin(int tid);

// Semáforo
int csem_init(csem_t *sem, int count);
// Semáforo
int cwait(csem_t *sem);
// Semáforo
int csignal(csem_t *sem);

int cidentify (char *name, int size);

TCB_t* searchTID(PFILA2 fila, int tid)
 /*Procura numa fila se existe o processo de tid e retorna | um ponteiro para o TCB caso positivo
  																												 | NULL caso contrário*/
{
    TCB_t* tcb;
    if (FirstFila2(fila)) // fila vazia
	{	return NULL;
    }
	while(*tcb = GetAtIteratorFila2(fila)))
	{	if(tcb->tid == tid)
		{	return tcb;
        }
		else
		{	if (NextFila2(fila))
			{	return NULL;
			}
		}
    }
	return NULL;
}


int inserirApto(TCB_t* thread){
		switch(thread->prio){
			case 0: //inserir no APTO_ALTA
						if(AppendFila2(APTO_ALTA,thread)) // RETORNA 0 CASO DEU SUCESSO
							return -1;
						else
							return 0;
						break;
			case 1: //inserir no APTO_MEDIA
						if(AppendFila2(APTO_MEDIA,thread))
							return -1;
						else
							return 0;
						break;
			case 2: //inserir no APTO_BAIXA
						if(AppendFila2(APTO_BAIXA,thread))
							return -1;
						else
							return 0;
						break;
			default:
						printf("A PRIORIDADE DO PROCESSO TA ZUADA!\n");
						return -1;
						break;
		}
}

int escalonador(){
	/*
	* Implementação de um escalonador preemptivo por prioridade
	*/
	TCB_t *proximo = pickHighestPriority();
	if(proximo == NULL){
		printf("A fila de aptos se encontra vazia!\n");
		return -1; // -1 indica que a fila de aptos está vazia
	}
	if(proximo->prio > EXEC->prio){
		despachante(proximo);
		return 0; //retorna 0 caso a troca de contexto tenha ocorrido.
	}
	return 1; //retorna 1 caso não tenha ocorrido troca de contexto.

}

TCB_t* pickHighestPriority(){
	TCB_t *escolhido;
	if(FirstFila2(APTO_ALTA)){ // Verdadeiro se não tem nenhum elemento de prioridade alta
		if(FirstFila2(APTO_MEDIA)){ // Verdadeiro se não tem nenhum elemento de prioridade alta ou média:
			if(FirstFila2(APTO_BAIXA)){ //Verdadeiro se não tem nenhum elemento em nenhuma fila (alta, media e baixa)
				return -1;
			}
			else{ //Existe pelo menos um TCB de prioridade baixa.
				escolhido = GetAtIteratorFila2(APTO_BAIXA);
			}
		}
		else{//Existe pelo menos um TCB de prioridade media.
			escolhido = GetAtIteratorFila2(APTO_MEDIA);
		}
	}
	else{//Existe pelo menos um TCB de prioridade alta.
		escolhido = GetAtIteratorFila2(APTO_ALTA);
	}
	return escolhido;
}

despachante(TCB_t *proximo){
	int estado = EXEC->state; //o próximo estado do executando define qual tratamento ele receberá.
	switch(estado){
		case PROCST_APTO: //É necessário colocar o processo na fila de aptos.
								break;
		case PROCST_BLOQ: //é necessário colocar o processo na fila de bloqueados.
								break;
		case PROCST_TERMINO: //o processo foi terminado: desalocar o PCB.
								free(EXEC);
								//EXEC = NULL;
								proximo->state = PROCST_EXEC;
								EXEC = proximo;

								break;
		//case PROCST_APTO_SUS
		//case PROCST_BLOQ_SUS
	}
}

adicionarApto (TCB_t *tcb){
	/* Retorna 0 caso tenha funcionado e -1 cc.
	*/
		switch(tcb->prio){
			case 0:
					if(AppendFila2(APTO_ALTA,tcb))
						return -1;
					else
						return 0;
					break;

			case 1:
					if(AppendFila2(APTO_MEDIA,tcb))
						return -1;
					else
						return 0;
					break;

			case 2:
					if(AppendFila2(APTO_BAIXA,tcb))
						return -1;
					else
						return 0;
					break;
			default:
					return -1;
					break;
		}
}
/*deletarApto(TCB_t *tcb){

}
*/
