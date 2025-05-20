import random as rd
import time
from datetime import datetime

"""
    Gera matriz quadrada zerada
"""
def criarMatrizZerada(tamanho):
    resultado = []
    for i in range(tamanho):
        linhaAtual = []
        for j in range(tamanho):
            linhaAtual += [0]
        resultado += [linhaAtual]
    return resultado

"""
    Gera matriz quadrada com valores aleatórios
"""
def criarMatrizAleatoria(tamanho):
    resultado = []
    for i in range(tamanho):
        linhaAtual = []
        for j in range(tamanho):
            linhaAtual += [rd.random()]
        resultado += [linhaAtual]
    return resultado


"""
    Multiplica 2 matrizes quadradas e retorna o resultado em C
"""
def multiplicarMatriz(matrizA,matrizB,matrizC,tamanho):
    for linhaA in range(tamanho): 
        for colunaB in range(tamanho):
            for posTermoAtual in range(tamanho):
                matrizC[linhaA][colunaB] += matrizA[linhaA][posTermoAtual]*matrizB[posTermoAtual][colunaB]
    return matrizC

"""
    Pega a data e hora atual e retorna esses valores
"""
def pegarDataEHoraAtual():
    agora = datetime.now()
    return agora.year,agora.month,agora.day,agora.hour,agora.minute,agora.second

    
"""
    Salva as informacoes de cada tamanho de matriz
"""
def salvarInformacoes(tamanhoMatriz,listaTempos,tipoExecucao = "python"):
    ano,mes,dia,hora,minuto,segundo = pegarDataEHoraAtual()
    nomeArquivo = f"tempos_{tipoExecucao}_{tamanhoMatriz}.txt"
    tempoTotal = 0 # para calcular a media

    with open(nomeArquivo,'w') as arquivo:
        arquivo.write(f"data: {dia}/{mes}/{ano} {hora}:{minuto}:{segundo} \n")
        arquivo.write(f"tamanho da matriz: {tamanhoMatriz}X{tamanhoMatriz} , tempo em segundos para multiplicacao ({tipoExecucao}) em segundos: \n")
        for i in range(len(listaTempos)):
            arquivo.write(f'{i}: {listaTempos[i]}\n')
            tempoTotal += listaTempos[i]
        tempoMedio = tempoTotal/len(listaTempos)
        arquivo.write(f"tempo medio: {tempoMedio}")

def main():
    tamanhoInicial = 100
    tamanhoFinal = 200
    espacamento = 100
    iteracoes = 10
    tamanhos = []
    for i in range((tamanhoFinal - tamanhoInicial)//espacamento + 1):
        tamanhos += [int(tamanhoInicial + i*espacamento)]
    
    for tamanho in tamanhos:
        listaTempos = []
        for iteracao in range(iteracoes):
            print(f'tamanho: {tamanho} iteracao: {iteracao}\n')
            print("Criando matrizes....\n")
            matrizA = criarMatrizAleatoria(tamanho)
            matrizB = criarMatrizAleatoria(tamanho)
            print("Criando matriz C...\n")
            matrizC = criarMatrizZerada(tamanho)
            print("Comecando calculos...\n")
            
            # parte do calculo
            tempoInicial = time.time()
            multiplicarMatriz(matrizA,matrizB,matrizC,tamanho)
            tempoTotal = time.time() - tempoInicial
            # fim do calculo

            print(f'Tempo total para multiplicar 2 matrizes de tamanho {tamanho} X {tamanho}: {tempoTotal}')
            listaTempos += [tempoTotal]
        salvarInformacoes(tamanho,listaTempos,"python")

            


if __name__ == "__main__":
    main()