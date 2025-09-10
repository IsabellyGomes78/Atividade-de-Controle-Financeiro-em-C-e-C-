/*Aplicação para registrar receitas, despesas e gerar relatórios financeiros. System Calls:
Criação de arquivos de transações, escrita de movimentações financeiras, leitura para cálculos de
saldo, geração de relatórios mensais.*/

#include <stdio.h>    // Para entrada e saída padrão (printf, fprintf, sprintf)
#include <stdlib.h>   // Para exit(), atof()
#include <string.h>   // Para strlen(), strtok(), memset()

// --- Inclusões específicas do sistema operacional ---
#ifdef _WIN32
#include <windows.h>
#else
#include <fcntl.h>    // Para open()
#include <unistd.h>   // Para write(), read(), close(), lseek()
#include <sys/stat.h> // Para as permissões do modo
#endif

// Enum para definir o tipo de transação
typedef enum {
    RECEITA,
    DESPESA
} TipoTransacao;

// Estrutura para armazenar os dados de uma transação
typedef struct {
    TipoTransacao tipo;
    char descricao[100];
    double valor;
} Transacao;

// Função para exibir mensagens de erro e sair
void exibir_erro(const char* mensagem) {
    fprintf(stderr, "Erro: %s\n", mensagem);
    exit(1);
}

int main() {
    const char* nomeArquivo = "transacoes_c.txt";
    
    // --- Dados das movimentações financeiras ---
    Transacao transacoes[] = {
        {RECEITA, "Salario", 3500.00},
        {DESPESA, "Aluguel", 1200.00},
        {DESPESA, "Supermercado", 450.50},
        {RECEITA, "Trabalho Freelance", 800.00},
        {DESPESA, "Internet e TV", 149.90},
        {DESPESA, "Transporte", 180.00}
    };
    int num_transacoes = sizeof(transacoes) / sizeof(transacoes[0]);
    
    #ifdef _WIN32
        HANDLE manipuladorArquivo;
    #else
        int descritorArquivo;
    #endif

    // --- Passo 1: Criação do arquivo de transações ---
    printf("1. A criar/abrir o ficheiro de transacoes '%s'...\n", nomeArquivo);
    #ifdef _WIN32
        manipuladorArquivo = CreateFileA(nomeArquivo, GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (manipuladorArquivo == INVALID_HANDLE_VALUE) exibir_erro("Nao foi possivel criar o ficheiro no Windows.");
    #else
        descritorArquivo = open(nomeArquivo, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (descritorArquivo == -1) exibir_erro("Nao foi possivel criar o ficheiro em sistema POSIX.");
    #endif
    printf("   -> Ficheiro criado/aberto com sucesso.\n");

    // --- Passo 2: Escrita das movimentações financeiras ---
    printf("\n2. A escrever movimentacoes financeiras no ficheiro...\n");
    for (int i = 0; i < num_transacoes; i++) {
        char linha[256];
        sprintf(linha, "%s,%s,%.2f\n",
                (transacoes[i].tipo == RECEITA ? "RECEITA" : "DESPESA"),
                transacoes[i].descricao,
                transacoes[i].valor);
        
        #ifdef _WIN32
            DWORD bytesEscritos;
            if (!WriteFile(manipuladorArquivo, linha, strlen(linha), &bytesEscritos, NULL)) {
                CloseHandle(manipuladorArquivo);
                exibir_erro("Nao foi possivel escrever no ficheiro.");
            }
        #else
            ssize_t bytesEscritos = write(descritorArquivo, linha, strlen(linha));
            if (bytesEscritos == -1) {
                close(descritorArquivo);
                exibir_erro("Nao foi possivel escrever no ficheiro.");
            }
        #endif
    }
    printf("   -> Movimentacoes escritas com sucesso.\n");

    // --- Passo 3: Leitura para cálculos de saldo e Geração de Relatório ---
    printf("\n3. A ler ficheiro para calculo de saldo e geracao de relatorio...\n\n");
    
    #ifdef _WIN32
        SetFilePointer(manipuladorArquivo, 0, NULL, FILE_BEGIN);
    #else
        lseek(descritorArquivo, 0, SEEK_SET);
    #endif

    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));
    
    #ifdef _WIN32
        DWORD bytesLidos;
        if (!ReadFile(manipuladorArquivo, buffer, sizeof(buffer) - 1, &bytesLidos, NULL)) {
            CloseHandle(manipuladorArquivo);
            exibir_erro("Nao foi possivel ler o ficheiro.");
        }
    #else
        ssize_t bytesLidos = read(descritorArquivo, buffer, sizeof(buffer) - 1);
        if (bytesLidos == -1) {
            close(descritorArquivo);
            exibir_erro("Nao foi possivel ler o ficheiro.");
        }
    #endif

    // --- Processamento e Geração do Relatório ---
    double total_receitas = 0;
    double total_despesas = 0;

    printf("--- RELATORIO FINANCEIRO MENSAL ---\n");
    
    // Usa strtok para obter cada linha
    char* linha = strtok(buffer, "\n");
    while (linha != NULL) {
        char tipo_str[100];
        char desc_str[100];
        double valor;

        // *** CORREÇÃO AQUI: Usa sscanf para analisar a linha de forma segura ***
        // Formato: lê uma string até à vírgula, outra string até à vírgula, e um double
        if (sscanf(linha, "%99[^,],%99[^,],%lf", tipo_str, desc_str, &valor) == 3) {
            if (strcmp(tipo_str, "RECEITA") == 0) {
                total_receitas += valor;
                printf("  [+] Receita: %-25s | Valor: R$ %.2f\n", desc_str, valor);
            } else {
                total_despesas += valor;
                printf("  [-] Despesa: %-25s | Valor: R$ %.2f\n", desc_str, valor);
            }
        }
        // Pega a próxima linha. Isto agora funcionará corretamente.
        linha = strtok(NULL, "\n");
    }
    
    printf("------------------------------------\n");
    printf("Total de Receitas: R$ %.2f\n", total_receitas);
    printf("Total de Despesas: R$ %.2f\n", total_despesas);
    printf("Saldo Final:       R$ %.2f\n", total_receitas - total_despesas);
    printf("------------------------------------\n");

    // --- Passo 4: Fechar o Arquivo ---
    printf("\n4. A fechar o ficheiro...\n");

    #ifdef _WIN32
        CloseHandle(manipuladorArquivo);
    #else
        close(descritorArquivo);
    #endif

    printf("   -> Ficheiro fechado com sucesso.\n");

    return 0;
}

