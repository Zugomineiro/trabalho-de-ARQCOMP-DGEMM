import random as rd
import time
from datetime import datetime
import subprocess

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
    diretorio = "tempos\\"
    tempoTotal = 0 # para calcular a media

    with open(diretorio + nomeArquivo,'w') as arquivo:
        arquivo.write(f"data: {dia}/{mes}/{ano} {hora}:{minuto}:{segundo} \n")
        arquivo.write(f"tamanho da matriz: {tamanhoMatriz}X{tamanhoMatriz} , tempo em segundos para multiplicacao ({tipoExecucao}) em segundos: \n")
        for i in range(len(listaTempos)):
            arquivo.write(f'{i}: {listaTempos[i]}\n')
            tempoTotal += listaTempos[i]
        tempoMedio = tempoTotal/len(listaTempos)
        arquivo.write(f"tempo medio: {tempoMedio}")

"""
    Escreve o tamanhoAtual no codigo e compila ele
"""
def configurarTamanhoC(tamanho,nomeArquivoC,nomeArquivoEXE,tipoO=0):
    with open("multiplicacaoParametros.h","w") as arquivo:
        arquivo.write(f"#define dimensaoMatriz {tamanho}\n")
        arquivo.write("#define tamanhoMatriz dimensaoMatriz*dimensaoMatriz")
    # Compila o arquivo
    argumentoExtra = ""
    if nomeArquivoEXE == "cap3.exe":
        argumentoExtra = "-mavx512f" # Para rodar vetores de 512 bytes no processador (nem todos os processadores podem lidar com essa operação)

    comando = "gcc -O" + str(tipoO) + " " + argumentoExtra + " " + nomeArquivoC + " -o " + nomeArquivoEXE
    subprocess.run(comando,shell=True)

"""
    Pega o resultado da iteracao
"""
def pegarTempoExecucao():
    with open("tempoExecucaoC.txt",'r') as arquivo:
        tempoExecucao = float(arquivo.readline().split(":")[1])
    return tempoExecucao

def main():
    tamanhoInicial = 1000
    tamanhoFinal = 8000
    espacamento = 1000
    iteracoes = 10
    arquivoExe = "cap3.exe"
    arquivoC = "multiplicacaoMatrizesCap3.c"
    tipoSimulacao = "Cap3"
    tiposO = [0,1,2,3]

    tamanhos = []

    for i in range((tamanhoFinal - tamanhoInicial)//espacamento + 1):
            tamanhos += [int(tamanhoInicial + i*espacamento)]

    for oAtual in tiposO:
        tipoO = oAtual
        tipoSimulacaoAtual = tipoSimulacao + "_O" + str(tipoO) + "_"

        tamanhosAtual = tamanhos
        # if oAtual == 3:
        #     tamanhosAtual = tamanhos[5:] # Os tamanhos anteriores ja foram processados
        
        for tamanho in tamanhosAtual:
            listaTempos = []
            configurarTamanhoC(tamanho,arquivoC,arquivoExe,tipoO)
            for iteracao in range(iteracoes):
                subprocess.run(".\\" + arquivoExe)

                tempoAtualExecucao = pegarTempoExecucao()
                listaTempos += [pegarTempoExecucao()]

                
            salvarInformacoes(tamanho,listaTempos,tipoSimulacaoAtual)

            


if __name__ == "__main__":
    main()