// Trabalho desenvolvido no Ubuntu 20.04.02 LTS 
// Nome: Felipe de Sa Monteiro de Barros Magrassi
// DRE: 118140750
// codigo pro exec e fork obtidos de: https://pages.cs.wisc.edu/~remzi/OSTEP/cpu-api.pdf
// codigo do signal handler obtido no site: https://www.thegeekstuff.com/2012/03/catch-signals-sample-c-code/
// Codigo para gerar numero aleatorio obtido de: https://www.tutorialspoint.com/c_standard_library/c_function_srand.htm
// Codigo do pipe obtido em: https://www.geeksforgeeks.org/c-program-demonstrate-fork-and-pipe/


#include<stdio.h>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <time.h>

void sig_handler(int signo)
{

    if (signo == SIGUSR1) { // Tarefa 1
        printf("\nFazendo Tarefa 1\n");
        
        pid_t pid = fork(); // realizando fork 
        if (pid < 0) { // teste de erro do fork
            printf("Fork Falhou. \n");
            exit(1);
        }
        else if (pid == 0) { // processo filho
        // array com o comando a ser executado
        char *processo[5] = {"ping", "8.8.8.8", "-c", "5", NULL}; 
        execvp(processo[0], processo);
        }
        else{
            wait(NULL);
            printf("Tarefa 1 Finalizada...Aguardando disparador \n");
        }
        
    }

    else if (signo == SIGUSR2) { // Tarefa 2    
    printf("\nFazendo Tarefa 2\n");   
    pid_t pid;
    int fd1[2]; // pipe de envio pai pro filho
    int fd2[2]; // pipe de envio filho pro pai
    time_t t;
    int sorteado, ler_sorteado, par;
    int n = 0;
    int valorRetorno = -1;
    srand((unsigned) time(&t));

// criar numero aleatorio e limitar ele ate 100
    while(valorRetorno != 0){
        n += 1;
        sorteado = rand() % 100;

        if (pipe(fd1)){ // criacao do primeiro pipe
            printf ("Pipe falhou.\n");
            exit(1);
        }

        if (pipe(fd2)){ // criacao do segundo pipe
            printf("Pipe falhou. \n");
            exit(1);
        }

        pid = fork(); // fork + teste 
        if (pid < 0) {
            printf("Fork Falhou");
            exit(2);
        } 
        // Processo do pai
        else if (pid > 0){ 
                // fechando pipe de leitura
                close(fd1[0]);
                // escrevendo para o filho e fechando o pipe
                write(fd1[1], &sorteado, sizeof(int));
                close(fd1[1]);

                // aguardando filho receber o valor
                wait(NULL);

                //fechando pipe de escrita
                close(fd2[1]);
                // leitura do filho e fechando o pipe
                read(fd2[0], &valorRetorno, sizeof(int));
                close(fd2[0]);

                printf("Numero da interacao: %d\n", n);
                printf("Numero sorteado: %d\n", sorteado);
                printf("Resposta do Filho: %d\n", valorRetorno);
                printf("\n");
                
        }
        // Processo do filho
        else {
            // fechando do pipe de escrita
            close(fd1[1]);
            // leitura do pipe 
            read(fd1[0], &ler_sorteado, sizeof(int));

            if(ler_sorteado % 2 == 0){ // verificar valor par ou impar
                par = 0;
            }
            else {
                par = 1;
            }
            // fechando dos pipes
            close(fd1[0]);
            close(fd2[0]);
            // escrevendo no pipe 2
            write(fd2[1], &par, sizeof(int));
            close(fd2[1]);
            // saindo do processo filho
            exit(0);   
        }
        }
        printf("\nFinalizando tarefa 2...Aguardando disparador.\n");
    }
    else if (signo == SIGTERM){    // Finalizar programa
        printf("Finalizando o disparador...\n");
        exit(0);
    }
}


int main(void)
{
    // print da PID do processo
    printf("Meu PID é: %d \n", (int) getpid());
    // handler de erros dos sinais
    if (signal(SIGUSR1, sig_handler) == SIG_ERR)
        printf("\nErro no sinal USR1\n");
    if (signal(SIGUSR2, sig_handler) == SIG_ERR)
        printf("\nErro no sinal SIGUS2\n");
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
        printf("\nErro no sinal SIGTERM\n");
    // loop infinito para enviar os sinais
    while(1)
        sleep(1);
    return 0;
}