# RELATÓRIO

1. Termo de compromisso

Os membros do grupo afirmam que todo o código desenvolvido para este
trabalho é de autoria própria.  Exceto pelo material listado no item 3
deste relatório, os membros do grupo afirmam não ter copiado
material da Internet nem obtiveram código de terceiros.

2. Membros do grupo e alocação de esforço

Preencha as linhas abaixo com o nome e o e-mail dos integrantes do
grupo.  Substitua XX pela contribuição de cada membro do grupo no
desenvolvimento do trabalho.

Gabriel Franco Jallais <gfjallais@ufmg.br> 50%
Henrique Daniel de Sousa <hds2021@ufmg.br> 50%

3. Referências bibliográficas

SILBERSCHATZ, Abraham. Fundamentos de Sistemas Operacionais. 8. ed. Rio de 
Janeiro: LTC, 2010.

Documentação das bibliotecas utilizadas.

4. Estruturas de dados

Descreva e justifique as estruturas de dados e algoritmos utilizados
em sua solução.

## sh.c

### Execução de Comandos (exec)

* Estruturas de Dados:

    struct execcmd: Armazena informações sobre comandos simples, principalmente seus argumentos (argv).

* Algoritmo:

    A função runcmd verifica o tipo de comando (cmd->type) e, em caso de um comando simples (' '), converte a estrutura cmd para struct execcmd.
    A execução de comandos simples é realizada pela chamada do sistema execvp, que substitui a imagem do processo pelo programa especificado no primeiro argumento (ecmd->argv[0]).

### Redirecionamento (redirecionamento)

* Estruturas de Dados:

    struct redircmd: Armazena informações sobre comandos de redirecionamento, incluindo o tipo de redirecionamento (type), o comando a ser executado (cmd), o nome do arquivo (file), o modo de abertura do arquivo (mode), e o número do descritor de arquivo (fd).

* Algoritmo:
    A função runcmd trata comandos de redirecionamento ao detectar que o tipo de comando é '<' ou '>'.
    Abre o arquivo especificado (file) com base no tipo de redirecionamento e modo de abertura (mode).
    Usa a chamada de sistema dup2 para redirecionar o descritor de arquivo (fd) para o arquivo recém-aberto.
    Chama recursivamente runcmd para executar o comando dentro do contexto do redirecionamento.


### Pipes (pipe)

* Estruturas de Dados:
    struct pipecmd: Armazena informações sobre comandos com pipes, incluindo o tipo de comando (type) e ponteiros para os comandos à esquerda (left) e à direita (right) do pipe.

* Algoritmo:

    A função runcmd trata comandos com pipes ao detectar que o tipo de comando é '|'.
    Usa a chamada de sistema pipe para criar um pipe.
    Realiza um fork para criar processos filho e pai.
    No processo filho, fecha o descritor de leitura (pipe_fd[0] ou STDIN_FILENO) e redireciona a saída padrão para o descritor de escrita do pipe (pipe_fd[1]).
    No processo pai, fecha o descritor de escrita (pipe_fd[1] ou STDOUT_FILENO) e redireciona a entrada padrão para o descritor de leitura do pipe (pipe_fd[0]).
    Chama recursivamente runcmd para os lados esquerdo e direito do pipe.

### Conclusão
Essa aplicação abrange a implementação eficiente de um shell para o UNIX, de modo que ele possa ser espandido à medida que o desenvolvedor desejar adicionar mais funcionalidades.
O uso de chamadas de sistema, como execvp, open, dup2, e pipe, reflete uma abordagem direta e eficiente para implementar as funcionalidades desejadas. A recursividade em runcmd é utilizada para lidar com a execução de comandos encadeados, redirecionamento e pipes, mantendo um código organizado e modular.

## meutop.c

* Estruturas de Dados:

Mutex (pthread_mutex_t lock): Um mutex é usado para garantir a exclusão 
mútua entre as threads. Isso é essencial, pois a operação de atualização 
da tabela interfere na obtenção da entrada do usuário.
Thread (pthread_t): Para exercitar os conceitos aprendidos em sala e realizar
operações de escrita e obtenção de entrada concorrentemente.

Além disso utilizamos estruturas de dados auxiliares de outras bibliotecas
para a leitura do diretório /proc e seus subdiretórios, e para identificar se o
usuário está tentando fornecer uma entrada.

* Algoritmos:

Listagem de Processos: O programa itera sobre os diretórios em "/proc" para 
encontrar processos em execução. Para cada processo encontrado, ele lê informações 
do arquivo /proc/<pid>/stat para obter o nome do processo e seu estado. Ele também 
lê o arquivo /proc/<pid>/status para obter o UID do usuário associado ao processo. 
O processo é então impresso na tela usando printf().

Envio de Sinais: A função get_signal() aguarda a entrada do usuário (pid do processo e número do sinal)
e envia o sinal usando a função kill(). Aqui, o algoritmo principal é a chamada do sistema kill(), 
que é usada para enviar sinais para processos.

O programa implementado é um exemplo prático de concorrência e manipulação de arquivos em sistemas Linux. 
Ele demonstra o uso de threads para realizar tarefas simultâneas - uma para exibir informações do processo 
na tela e outra para capturar os sinais do usuário. A utilização de mutex garante que a saída seja bem 
coordenada entre essas threads, evitando condições de corrida.

Além disso, o programa explora a estrutura do sistema de arquivos /proc do Linux para obter 
informações detalhadas sobre os processos em execução. Isso ilustra como as informações do
sistema podem ser acessadas e utilizadas em programas C.

A implementação do programa também mostra um bom entendimento do uso de chamadas de sistema,
manipulação de arquivos e manipulação de strings em C.