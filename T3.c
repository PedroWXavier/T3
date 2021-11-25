#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <stdio.h>
#define ARRAY_SIZE 1000000

using namespace std;

void BubbleSort(int ini, int fim, int * vetor)
{
    int c, d, troca;

    for (c=ini; c<fim-1; c++){
        for (d = c ; d < fim-1; d++)
            if (vetor[d] > vetor[d+1]){
                troca      = vetor[d];
                vetor[d]   = vetor[d+1];
                vetor[d+1] = troca;
                trocou = 1;
            }
    }
}


int main(int argc, char** argv) {
    
    //Initialização
    double t1, t2;
    int intRank, proc_n;
    int tamVetor=ARRAY_SIZE;
    int vetor[ARRAY_SIZE];
    int indVetIni, indVetFim;


    MPI_Status status;
    MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&intRank);
    MPI_Comm_size(MPI_COMM_WORLD,&proc_n);
    t1 = MPI_Wtime();

    //Inicializa cada processo
    if(intRank==0){
        int i,;
        int acumulador=0;
        int indice=0;
        //Inicializa vetor com 1.000.000 de números em ordem decrescente
        printf("Inicializando vetor desordenado.");
        printf("...");
        for(i = ARRAY_SIZE-1; i>=0; i--){
            vetor[i] = i;
        }
        tamVetor = ARRAY_SIZE;
        printf("Vetor incializado.");
        
        //Distribui vetores dando índices
        indice = (tamVetor/proc_n) -1; //Ex: 100/4 -1 = 24
        for(i = 1; i<proc_n; i++){
            //envia posição incial e final do intervalo de cada processo.
            MPI_Send(&acumulador, 1, MPI_INT, i, 0, MPI_COMM_WORLD); //Ex: manda 0
            MPI_Send(&indice, 1, MPI_INT, i, 0, MPI_COMM_WORLD); //Ex: manda 24
            //atualiza indices para novo processo
            acumulador = indice + 1; //Ex: novo inicial vai ser o final anterior, 25
            indice = acumulador + (tamVetor/proc_n) -1; //Ex: novo final vai ser 25 + 24 = 49
            if(indice > tamVetor-1){ //Se o próximo intervalo supera o máximo, fica no máximo
                indice = tamVetor-1;
            }
        }



    } else { //Receber os intervalos do vetor global
        printf("Processo %d aguardando intervalo vetor.\n", intRank);
        MPI_Recv(&indVetIni, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);   
        printf("\tProcesso %d recebeu inicio do intervalo.\n", intRank);     
        MPI_Recv(&indVetFim, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status); 
        printf("\tProcesso %d recebeu fim do intervalo.\n", intRank);     
    }


    //Execução
    int go =1;
    while(go){
        if (intRank == 0) {// Controla se o vetor está totalmente ordenado ou não
            printf("Verificando se o programa encerrou: %d\n", );
            go = 0; //Assume que deu ok
            //Se algum processo não está ordenado com seus vizinhos, ele irá avisar.
            MPI_Request req;
            MPI_Irecv(&go, 1, MPI_INT, 0, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            time_t now = time(NULL);
            while(time(NULL) < now + 3){// 3 segundos
                over = 1;
                int flag = 0;
                MPI_Test(req, flag, status);
                if(flag){
                    over = 0;
                    MPI_Bcast(1, 1, MPI_INT, 0, MPI_COMM_WORLD); //Manda continuar
                    break; //exit timeout loop if something was received
                }
            }
            if(over){
                //Nenhum processo avisou nada, vetor ordenado
                go =0;
                //Manda parar
                MPI_Bcast(0, 1, MPI_INT, 0, MPI_COMM_WORLD);
            }


        } else { //É um dos processos de processamento
            int comparador, tamIntervalo, i=0, j=0, stop=1;
            int vetAuxTroca1[ARRAY_SIZE], vetAuxTroca2[ARRAY_SIZE];

            MPI_Bcast(&stop, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if(stop){
                //Organiza sua parte do vetor
                BubbleSort(indVetIni, indVetFim, &vetor)

                //Se não é a ponta da direita
                if (intRank != proc_n-1){ 
                    //Passa o maior para o vizinho direito com o tamanho do seu intervalo na tag
                    MPI_Send(&vetor[indVetFim], 1, MPI_INT, intRank +1, (indVetFim-indVetIni), MPI_COMM_WORLD);
                } 

                //Verifica se está em ordem com os vizinhos
                if(intRank>1){  //Se não é a ponta da esquerda 
                    //Recebe o maior do vizinho esquerdo com o tamanho do intervalo dele
                    MPI_Recv(&comparador, 1, MPI_INT, 0, intRank-1, tamIntervalo, &status);
                    //Se o maior do esquerdo é maior que o menor local, não está em ordem
                    if (comparador > vetor[indVetIni]){ 
                        //Avisa o controle
                        MPI_Request request;
                        MPI_Isend(1, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
                        
                        //---Realiza troca com o vizinho da Esquerda---
                        //Pega a sua primeira metade do intervalo
                        j=0;
                        for (i = indVetIni; i < indVetIni+ tamIntervalo/2 ; i++){
                            vetAuxTroca1[j] = vetor[i];
                            j++;
                        }                    
                        //Da sua primeira metade para o vizinho da esquerda
                        MPI_Send(&vetAuxTroca1, tamIntervalo/2, MPI_INT, intRank-1, 0, MPI_COMM_WORLD); 
                        //Recebe a segunda metade para o vizinho da esquerda
                        MPI_Recv(&vetAuxTroca2, tamIntervalo/2, MPI_INT, 0, intRank-1, 0, &status);
                        //Substitui sua primeira metade com a segunda do processo à esquerda
                        j=0;
                        for (i = indVetIni; i < indVetIni+ tamIntervalo/2 ; i++){
                            vetor[i] = vetAuxTroca2[j];
                            j++;
                        } 

                        //---Realiza troca com o vizinho da Direita---
                        if (intRank != proc_n-1){
                            //Pega a sua segunda metade do intervalo
                            j=0;
                            for (i = (indVetFim-indVetIni)/2; i < tamIntervalo; i++){
                                vetAuxTroca1[j] = vetor[i];
                                j++;
                            }                    
                            //Recebe a primeira metade do vizinho da direita
                            MPI_Recv(&vetAuxTroca2, (indVetFim-indVetIni)/2, MPI_INT, 0, intRank+1, 0, &status);
                            //Da sua segunda metade para o vizinho da direita
                            MPI_Send(&vetAuxTroca1, (indVetFim-indVetIni)/2, MPI_INT, intRank+1, 0, MPI_COMM_WORLD); 
                            //Substitui sua primeira metade com a segunda do processo à esquerda
                            j=0;
                            for (i = indVetIni; i < indVetIni+ tamIntervalo/2 ; i++){
                                vetor[i] = vetAuxTroca2[j];
                                j++;
                            } 
                        }
                        //Cancela o aviso ao mestre se não foi até agora, só importa o primeiro
                        MPI_Cancel(&request);
                    }
                }
            }
        }

        t2 = MPI_Wtime();
        double time=(t2-t1) ;
        printf("\nTempo de execução: %f\n\n", time);

    }
    MPI_Finalize();

    return 1;
 }



 /*
 ALGORITMO

Versão paralela seguindo o modelo fases paralelas, de um programa que ordena um grande vetor usando o algortimo Bubble Sort. 
Cada um dos processos é responsável por 1/np do vetor (neste caso 1/16), que já pode ser gerado internamente em cada processo,
     sem a necessidade de gerar em um único processo e depois distribuir entre os restantes. 
Depois, o processamento segue as seguintes fases, que são executadas em paralelo por todos os processos até a conclusão da ordenação (por isso o nome do modelo):

Ordenação local: cada processo ordena a sua parte do vetor global
É feita uma verificação distribuída para identificar se o vetor global esta ordenado:
    cada processo envia o seu maior valor para o vizinho da direita, este compara com o seu menor valor e verifica se os dois processos estão ordenados entre si. Como todos os processos fazem esta operação
        , cada um sabe se está ordenado em relação ao vizinho da esquerda. Esta informação é distribuída para todos os processos com uma operação de comunicação coletiva (Broadcast).
    Se todos estiverem ordenados entre si, todos terminam
Se não, cada processo troca uma parte dos seus valores mais baixos com os mais altos do vizinho da esquerda.
    A ideia é empurrar os valores que estão fora de lugar para os processos corretos, e volta para a fase 1.
------------------------------------------------------------------------------------------------------------------------------

0: Gera vetor global em ordem decrescente
1: Divide o vetor em trechos iguais para cada processo
Enquanto não encerrar:
    2: Cada processo aplica bubble sort no seu trecho do vetor.
    3: Verifica se todos os processos estão ordenados entre si, verificando se o seu número maior no trecho é menor que o menor do próximo trecho.
        3.5: Processos que não estiverem ordenados com o vizinho avisam o controle.
    4: Cada processo passa a primeira metade do seu trecho ordenado para o processo anterior e este devolve sua metade mais alta.




*/