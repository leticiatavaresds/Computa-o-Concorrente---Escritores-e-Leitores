# Trabalho 2 - Computação Concorrente

O trabalho é composto por duas partes. Sendo a primeira o programa principal, uma solução para o problema de escritores/leitores com garantia de ausencia de inanicção das threads. E a segunda, um programa auxiliar para verificar, através de uma leitura do log de execução gerado pelo programa principal, se a execução ocorreu com sucesso.

# Programa Principal

O programa principal é dado pelo arquivo "Escritoras e Leitoras.c" escrito na linguagem C.
    Para compilá-lo, deve-se baixar o arquivo e executá-lo, passando como argumentos na linha de comando os seguites parâmetros: "número de threads leitoras" "número de threads escritoras" "numero de leituras" "numero de escritas" "nome do arquivo de log,txt" sem as aspas.
    A saída será um arquivo "id.txt" para cada thread leitora contendo as variáveis lidas por ela, sendo id a identificação da thread, e o arquivo log com o nome igual ao passado como parâmetro.
	
# Programa Auxiliar

O programa auxiliar é dado pelo arquivo "Programa auxiliar.py" escrito na linguagem Python e implementado utilizando a plataforma Colab.
    Para compilá-lo, deve-se clicar no link dado pela imagem com a frase "open in colab" que leva para o colab e executá-lo seguindo as orientação contidas junto ao código.
    O programa tem como saída um texto descrevendo toda a análise das funções, mostrando se cada uma deveria ter ocorrido o não, e outro texto analisando se a execução dada do código "Escritores e Leitoras.c" satisfez as condições dadas pelo problema, sendo elas:
##### -Quantidade de Threads Leitoras igual ao número pedido pelo usuário.
##### -Quantidade de Threads EScritoras igual ao número pedido pelo usuário.
##### - Número de leituras realizadas por cada thread leitora igual ao número pedido pelo usuário.
##### - Número de escritas realizadas por cada escritora igual ao número pedido pelo usuário.
##### - Execução correta em todas as leituras, para isso não podia ter nenhuma escritora escrevendo quando uma leitora fosse ler.
##### -Execução correta em todas as escrtas, para isso não podia ter nenhuma escritora escrevendo nem leitora lendo quando uma escritora fosse escrever.
##### - Ausência de Inanição das threads.
