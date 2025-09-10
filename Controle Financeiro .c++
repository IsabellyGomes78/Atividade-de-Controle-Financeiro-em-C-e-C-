/*Conceito: Aplicação para registrar receitas, despesas e gerar relatórios financeiros. System Calls:
Criação de arquivos de transações, escrita de movimentações financeiras, leitura para cálculos de
saldo, geração de relatórios mensais.*/

// --- Inclusão das Bibliotecas Padrão ---
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>

// --- Definições de Tipos de Dados ---

// Define os tipos de transação possíveis (versão antiga para compatibilidade).
enum TipoTransacao {
    RECEITA,
    DESPESA
};

// Estrutura que representa uma única transação financeira.
struct Transacao {
    TipoTransacao tipo;
    std::string descricao;
    double valor;
};

// --- Funções Auxiliares ---

// Exibe uma mensagem de erro e encerra o programa.
void exibir_erro(const std::string& mensagem) {
    std::cerr << "Erro: " << mensagem << std::endl;
    exit(1);
}

// --- Função Principal ---
int main() {
    const std::string nomeArquivo = "transacoes_cpp.txt";
    
    // --- Preparação dos Dados ---
    // Cria um vetor para armazenar todas as transações.
    std::vector<Transacao> transacoes;
    Transacao temp;

    // Adiciona as transações ao vetor (método compatível com C++98).
    temp.tipo = RECEITA; temp.descricao = "Salario"; temp.valor = 3500.00;
    transacoes.push_back(temp);
    
    temp.tipo = DESPESA; temp.descricao = "Aluguel"; temp.valor = 1200.00;
    transacoes.push_back(temp);

    temp.tipo = DESPESA; temp.descricao = "Supermercado"; temp.valor = 450.50;
    transacoes.push_back(temp);

    temp.tipo = RECEITA; temp.descricao = "Trabalho Freelance"; temp.valor = 800.00;
    transacoes.push_back(temp);

    temp.tipo = DESPESA; temp.descricao = "Internet e TV"; temp.valor = 149.90;
    transacoes.push_back(temp);
    
    temp.tipo = DESPESA; temp.descricao = "Transporte"; temp.valor = 180.00;
    transacoes.push_back(temp);

    // --- Passo 1 e 2: Criação e Escrita no Ficheiro ---
    std::cout << "1 & 2. A criar e escrever no ficheiro de transacoes '" << nomeArquivo << "'...\n";
    { // O uso de um escopo {} garante que o ficheiro é fechado automaticamente no final.
        // O método .c_str() é usado para compatibilidade com compiladores antigos.
        std::ofstream arquivoSaida(nomeArquivo.c_str());
        if (!arquivoSaida.is_open()) {
            exibir_erro("Nao foi possivel criar o ficheiro para escrita.");
        }

        // Garante que os valores são escritos com 2 casas decimais.
        arquivoSaida << std::fixed << std::setprecision(2);

        // Itera sobre o vetor para escrever cada transação no ficheiro.
        for (std::vector<Transacao>::const_iterator it = transacoes.begin(); it != transacoes.end(); ++it) {
            const Transacao& transacao = *it;
            arquivoSaida << (transacao.tipo == RECEITA ? "RECEITA" : "DESPESA") << ","
                         << transacao.descricao << ","
                         << transacao.valor << "\n";
        }
    } // Fim do escopo, o ficheiro de escrita é fechado.
    std::cout << "   -> Movimentacoes escritas com sucesso.\n";

    // --- Passo 3: Leitura, Cálculo e Geração do Relatório ---
    std::cout << "\n3. A ler ficheiro para calculo de saldo e geracao de relatorio...\n\n";
    
    // Variáveis para acumular os totais.
    double total_receitas = 0;
    double total_despesas = 0;
    
    { // Escopo para garantir o fecho automático do ficheiro de leitura.
        std::ifstream arquivoEntrada(nomeArquivo.c_str());
        if (!arquivoEntrada.is_open()) {
            exibir_erro("Nao foi possivel abrir o ficheiro para leitura.");
        }

        std::cout << "--- RELATORIO FINANCEIRO MENSAL ---\n";
        
        std::string linha;
        // Lê o ficheiro linha por linha.
        while (std::getline(arquivoEntrada, linha)) {
            std::stringstream ss(linha); // Permite tratar a linha como um fluxo de dados.
            std::string tipo_str, desc_str, valor_str;

            // Extrai os campos da linha separados por vírgula.
            if (std::getline(ss, tipo_str, ',') && std::getline(ss, desc_str, ',') && std::getline(ss, valor_str)) {
                double valor;
                std::stringstream conversor(valor_str);
                if (conversor >> valor) { // Tenta converter a string do valor para um número.
                    if (tipo_str == "RECEITA") {
                        total_receitas += valor;
                        std::cout << "  [+] Receita: " << std::left << std::setw(25) << desc_str 
                                  << " | Valor: R$ " << valor << "\n";
                    } else {
                        total_despesas += valor;
                        std::cout << "  [-] Despesa: " << std::left << std::setw(25) << desc_str 
                                  << " | Valor: R$ " << valor << "\n";
                    }
                } else {
                    std::cerr << "Aviso: Nao foi possivel converter valor na linha -> " << linha << std::endl;
                }
            }
        }
    } // Fim do escopo, o ficheiro de leitura é fechado.

    // --- Exibição do Resumo Final ---
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "------------------------------------\n";
    std::cout << "Total de Receitas: R$ " << total_receitas << "\n";
    std::cout << "Total de Despesas: R$ " << total_despesas << "\n";
    std::cout << "Saldo Final:       R$ " << (total_receitas - total_despesas) << "\n";
    std::cout << "------------------------------------\n";

    std::cout << "\n4. Processo concluido. Os ficheiros foram fechados automaticamente.\n";

    // Retorna 0 para indicar que o programa terminou com sucesso.
    return 0;
}