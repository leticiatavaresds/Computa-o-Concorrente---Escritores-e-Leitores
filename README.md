# Escritores e Leitores com Garantia de Ausencia de Inanição


![GitHub repo size](https://img.shields.io/github/repo-size/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores?color=a21360&style=for-the-badge)
![GitHub language count](https://img.shields.io/github/languages/count/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores?color=a21360&style=for-the-badge)
![Made With](https://img.shields.io/badge/Made%20With-C;%20Python-lightgrey?color=a21360&style=for-the-badge)
![GitHub repo file count](https://img.shields.io/github/directory-file-count/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores?color=a21360&style=for-the-badge)
![GitHub last commit](https://img.shields.io/github/last-commit/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores?color=a21360&style=for-the-badge)


# Índice

- [Introdução](#introdução)
- [Programa Principal](#programa-principal)
- [Programa Auxiliar](#programa-auxiliar)
- [Solução Implementada](#solução-implementada)
	- [Função Leitura](#função-leitura)
	- [Função Escrita](#função-escrita)
- [Licença](#licença)

# Introdução

O problema dado de Leitras e Escritoras é caracterizado pela necessidade de threads de dois tipos, leitoras e escritoras, compartilharem um mesmo recurso, que nesse caso é uma única váriável correspondente ao número de identificação da thread que a escreveu.
A execução deve seguir três regras: 
- Qualquer número de leitores pode ler do recurso compartilhado simultaneamente, mas apenas um escritor escreve por vez. 
- Quando uma escritora está escrevendo, nenhuma outra thread pode ter acesso a variável compartilhada. 
- Uma escritora não pode escrever se houver algum leitor lendo no momento. Da mesma forma, um leitor não pode ler se existe um escritor escrevendo. 

A proposta do trabalho é implemetar um programa usando a biblioteca pthread na linguam ```C```, que consiga atender as exigências acima, além de garantir a inanição (Starvation) de threads. O trabalho é composto por duas partes. Sendo a primeira o programa principal, uma solução para o problema de escritores/leitores com garantia de ausencia de inanição das threads. E a segunda, um programa auxiliar para verificar, através de uma leitura do log de execução gerado pelo programa principal, se a execução ocorreu com sucesso.

# Programa Principal

O programa principal é dado pelo arquivo [Escritoras e Leitoras.c](https://github.com/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores/blob/master/Leitoras%20e%20Escritoras.c) escrito na linguagem ```C```.
    Para compilá-lo, deve-se baixar o arquivo e executá-lo, passando como argumentos na linha de comando os seguites parâmetros: "número de threads leitoras" "número de threads escritoras" "numero de leituras" "numero de escritas" "nome do arquivo de log,txt" sem as aspas.
    A saída será um arquivo "id.txt" para cada thread leitora contendo as variáveis lidas por ela, sendo id a identificação da thread, e o arquivo log com o nome igual ao passado como parâmetro.
    
# Programa Auxiliar

O programa auxiliar é dado pelo arquivo [Programa auxiliar.ipynb](https://github.com/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores/blob/master/Programa%20auxiliar.ipynb) escrito na linguagem ```Python``` e implementado utilizando a plataforma ```Colab```.
Para compilá-lo, deve-se clicar no link dado pela imagem com a frase "open in colab" que leva para o colab e executá-lo seguindo as orientação contidas junto ao código.
    O programa tem como saída um texto descrevendo toda a análise das funções, mostrando se cada uma deveria ter ocorrido ou não, além de uma análise se a execução dada do código [Escritoras e Leitoras.c](https://github.com/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores/blob/master/Leitoras%20e%20Escritoras.c) satisfez as condições dadas pelo problema, sendo elas:
- Quantidade de Threads Leitoras igual ao número pedido pelo usuário.
- Quantidade de Threads Escritoras igual ao número pedido pelo usuário.
- Número de leituras realizadas por cada thread leitora igual ao número pedido pelo usuário.
- Número de escritas realizadas por cada escritora igual ao número pedido pelo usuário.
- Execução correta em todas as leituras, para isso não pode-se ter nenhuma escritora escrevendo quando uma leitora realiza a leitura.
- Execução correta em todas as escrtas, para isso não pode-se ter nenhuma escritora escrevendo nem leitora lendo enquanto uma escritora escreve.
- Ausência de Inanição das threads.


# Solução Implementada

Para conseguir executar com êxito, foi tilizado nesse trabalho dois mecanismos de sincronização:
- Mutex (pthread_mutex_t) - Bloqueia o acesso às variáveis por outros threads.
- Variáveis de condição - (pthread_cond_t): permite que as threads suspendam a execução e abandonem o processador até que alguma condição seja verdadeira.

## Função Leitura
``` c
while (a<NLEI) {
		usleep(1);
		leit[j]=comeca_leitura(i);
		termina_leitura(i);
		a++;
		j++;
	}
```
A função leitura implementada pelas threads leitoras é dada em duas partes. 

- Primeira parte: a thread utiliza o mutex ao entrar na função e ao chegar na sessão crítica, verifica se as condições necessárias para que ela leia estão sendo atendidas, se não, fica bloqueada pela variável de condição até que uma escritora termine uma escrita e libere o sinal que pode permitir que ela prossiga. Ao ser bloqueada, a thread entra na fila de leitoras, estrutura implementada pela variável `l_esp` que indica quantas leitoras estão esperando para ler. Ao receber o sinal emitido por uma escritora, a leitora confere se as condições estão sendo atendidas antes de sair, caso não esteja, caso que pode ocorrer já que uma outra thread pode ter sido mais rápida, a thread volta pra fila.

Caso as condições estejam sendo satisfeitas, a leitora sai da fila e decrementa a variável `l_esp`. Começa assim a leitura, onde a thread lê o valor da variável compartilhada e escreve em um arquivo que leva o nome de sua id (variável de identificação da thread), encerrando assim a primeira parte da leitura. A parte do código descrita pode ser vista abaixo, as partes de escrita no arquivo log foram tiradas para uma melhor compreensão.

``` c
    int comeca_leitura(int i) {

	int leitura;
	pthread_mutex_lock(&mutex);
	nlida++;

	//Se houver alguma escritora escrevendo ou uma ou mais escritoras escrevendo, vai pra fila de leitoras
	if (e_cont == 1 || e_esp > 0) {
		// incrementa o número de leitoras esperando
		l_esp++;
		// fica na fila até receber o sinal de que pode ler
		while (1) {
			pthread_cond_wait(&podeler, &mutex);
			if (e_cont == 0) break;
		}
		l_esp--;
	}

	
	fprintf(arq, "\n%d",var);
	l_cont++;
	leitura = var;
	pthread_mutex_unlock(&mutex);
	return leitura;
};
```
- Segunda Parte: a thread leitora encerra sua leitura chamando a função "termina_leitura", sendo essa parte de grande importância para a garantia de ausência de inanição, já que a ideia implantada foi a de uma thread de um tipo dar prioridade para threads de outro tipo. Sendo assim, quando uma thread leitora termina sua leitura, ela informa que não está mais lendo, decrementando a variável 'l_cont', e manda sinal APENAS para as escritoras.

O código pode ser visto abaixo:
```c
void termina_leitura(int i)
{
	pthread_mutex_lock(&mutex);

	//Informa qe parou de ler (--l_cont)
	//Se não houver mais leitoras lendo, libera as escritoras
	if (--l_cont == 0){
		pthread_cond_signal(&pode_escr);
	}
	pthread_mutex_unlock(&mutex);
};
```
## Função Escrita

Assim como a função leitura, a função escrita implementada pelas threads escritoras também é dada em duas partes. Sendo o processo de escrita muito parecido com o de leitura.

- Primeira parte: a thread utiliza o mutex ao entrar na função e ao chegar na sessão critíca, verifica se as condições necessárias para que ela leia estão sendo atendidas, se não, fica bloqueada pela variável de condição até que uma leitora termine de ler ou uma escritora termine de escrever e libere o sinal que pode permitir que ela prossiga. Ao ser bloqueada, a thread entra na fila de escritoras, estrutura implementada pela variável `e_esp` que indica quantas escritoras estão esperando para ler. Ao receber o sinal, a escritora confere se as condições estão sendo atendidas antes de sair, caso não esteja, a thread volta pra fila.

Caso as condições estejam sendo satisfeitas, a escritora sai da fila, incrementa a variável `e_cont` indicando que tem uma escritora escrevendo e realiza a escrita, atribuindo o valor da sua id (variável de identificação da thread) à variável compartilhada, encerrando assim a primeira parte da escrita. A parte do código descrita pode ser vista abaixo, as partes de escrita no arquivo log foram tiradas para uma melhor compreensão.

``` c
void comeca_escrita(int i)
{
	// a escritora mais rápida será dona da sessão crítica
	pthread_mutex_lock(&mutex);
	nescrita++;
	//se tiver outra escritoras escrevendo ou uma ou mais leitora lendo,  ela espera até receber o sinal
	if (e_cont == 1 || l_cont > 0) {
		++e_esp;
		while(1){
			pthread_cond_wait(&pode_escr, &mutex);
			if (e_cont == 0 && l_cont == 0) break;

		}
		--e_esp;
	}
	//Foi liberada, pode escrever
	//Informa que tem uma escritora escrevendo:
	e_cont = 1;
	var = i;
	pthread_mutex_unlock(&mutex);
};
```

- Segunda Parte: a thread escritora encerra sua leitura chamando a função "para_escrita", sendo novamente essa parte de grande importância para a garantia de ausência de inanição, já que aqui também segue a ideia de prioridade para a thread do outro tipo. A thread escritora quando termina sua escrita,  informa que não está mais lendo, decrementando a variável 'e_cont', e manda sinal APENAS para as leitoras (broadcast nesse caso, já que mais de uma leitora pode ler simultâneamente). Somente no caso da fila de leitoras estar vazia, a thread escritora manda um sinal pra liberar uma escritora.

```c
void para_escrita(int i)
{
	pthread_mutex_lock(&mutex);
	//Informa que não tem mais thread escreved
	e_cont = 0;
	//se tem leitores esperando, elas são liberadas, se não, as escritoras são liberadas
	if (l_esp > 0){
		pthread_cond_broadcast(&podeler);
	}
		//pthread_cond_signal(&podeler);
	else{
		pthread_cond_signal(&pode_escr);
	}

	pthread_mutex_unlock(&mutex);
};
```


# Licença

The MIT License (MIT) 2022 - Letícia Tavares. Leia o arquivo [LICENSE.md](https://github.com/leticiatavaresds/Computacao-Concorrente---Escritores-e-Leitores/blob/master/LICENSE.md) para mais detalhes.

[⬆ Voltar ao topo](#escritores-e-leitores-com-garantia-de-ausencia-de-inanição)<br>
