/*
 * CCP230 - Desenvolvimento de Algoritmos
 * Projeto: Exchange de Criptomoedas (versão individual)
 *
 * Arquivos de texto usados (criados automaticamente se não existirem):
 *   usuarios.txt        -> cpf;senha;nome;reais;btc;eth;xrp
 *   cotacoes.txt        -> cotação de BTC, ETH e XRP (em reais)
 *   extrato_<CPF>.txt   -> extrato de operações de cada investidor
 *
 * Compilar: gcc exchange.c -o exchange
 *
 * Obs.: salve este arquivo como UTF-8 para os acentos aparecerem
 *       corretamente no console do Windows (Dev-C++).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifdef _WIN32
#include <windows.h>   /* SetConsoleOutputCP, para exibir acentos no console */
#endif

#define MAX_USUARIOS 100
#define ARQ_USUARIOS "usuarios.txt"
#define ARQ_COTACOES "cotacoes.txt"

/* Taxas de compra e venda de cada criptomoeda */
#define TX_COMPRA_BTC 0.02
#define TX_VENDA_BTC  0.03
#define TX_COMPRA_ETH 0.01
#define TX_VENDA_ETH  0.02
#define TX_COMPRA_XRP 0.01
#define TX_VENDA_XRP  0.01

typedef struct {
    char cpf[12];
    char senha[7];
    char nome[50];
    double reais;
    double btc;
    double eth;
    double xrp;
} Usuario;

typedef struct {
    double btc;
    double eth;
    double xrp;
} Cotacoes;

Usuario usuarios[MAX_USUARIOS];
int totalUsuarios = 0;
Cotacoes cot;

/* ------------------------------------------------------------------ */
/* Utilitários de entrada                                             */
/* ------------------------------------------------------------------ */

void lerLinha(char *buf, int tam) {
    if (fgets(buf, tam, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    buf[strcspn(buf, "\r\n")] = '\0';
}

/* Lê um número real positivo (aceita vírgula ou ponto). Retorna -1 se inválido */
double lerValor(const char *msg) {
    char buf[64];
    char *fim;
    double v;
    int i;

    printf("%s", msg);
    lerLinha(buf, sizeof(buf));
    for (i = 0; buf[i]; i++)
        if (buf[i] == ',') buf[i] = '.';
    if (buf[0] == '\0') return -1;
    v = strtod(buf, &fim);
    if (*fim != '\0' || v <= 0) return -1;
    return v;
}

int lerOpcao(void) {
    char buf[16];
    lerLinha(buf, sizeof(buf));
    if (buf[0] == '\0') return -1;
    return atoi(buf);
}

int somenteDigitos(const char *s, int tamanho) {
    int i;
    if ((int)strlen(s) != tamanho) return 0;
    for (i = 0; i < tamanho; i++)
        if (!isdigit((unsigned char)s[i])) return 0;
    return 1;
}

/* Imprime número com vírgula decimal (ex.: 580,00) */
void fmt(char *dest, double v, int casas) {
    char *p;
    sprintf(dest, "%.*f", casas, v);
    for (p = dest; *p; p++)
        if (*p == '.') *p = ',';
}

void formatarCPF(const char *cpf, char *dest) {
    sprintf(dest, "%.3s.%.3s.%.3s-%.2s", cpf, cpf + 3, cpf + 6, cpf + 9);
}

/* ------------------------------------------------------------------ */
/* Persistência em arquivos                                           */
/* ------------------------------------------------------------------ */

void salvarUsuarios(void) {
    FILE *f = fopen(ARQ_USUARIOS, "w");
    int i;
    if (!f) {
        printf("Erro ao salvar %s!\n", ARQ_USUARIOS);
        return;
    }
    for (i = 0; i < totalUsuarios; i++) {
        Usuario *u = &usuarios[i];
        fprintf(f, "%s;%s;%s;%.2f;%.8f;%.8f;%.8f\n",
                u->cpf, u->senha, u->nome, u->reais, u->btc, u->eth, u->xrp);
    }
    fclose(f);
}

void salvarCotacoes(void) {
    FILE *f = fopen(ARQ_COTACOES, "w");
    if (!f) {
        printf("Erro ao salvar %s!\n", ARQ_COTACOES);
        return;
    }
    fprintf(f, "%.2f %.2f %.4f\n", cot.btc, cot.eth, cot.xrp);
    fclose(f);
}

void criarUsuarioPadrao(const char *cpf, const char *senha, const char *nome) {
    Usuario *u = &usuarios[totalUsuarios++];
    strcpy(u->cpf, cpf);
    strcpy(u->senha, senha);
    strcpy(u->nome, nome);
    u->reais = u->btc = u->eth = u->xrp = 0.0;
}

void carregarDados(void) {
    FILE *f;
    char linha[256];

    /* Usuários */
    f = fopen(ARQ_USUARIOS, "r");
    if (f) {
        while (totalUsuarios < MAX_USUARIOS && fgets(linha, sizeof(linha), f)) {
            Usuario *u = &usuarios[totalUsuarios];
            if (sscanf(linha, "%11[^;];%6[^;];%49[^;];%lf;%lf;%lf;%lf",
                       u->cpf, u->senha, u->nome,
                       &u->reais, &u->btc, &u->eth, &u->xrp) == 7)
                totalUsuarios++;
        }
        fclose(f);
    }
    if (totalUsuarios == 0) {
        /* Primeiro uso: cria investidores de exemplo */
        criarUsuarioPadrao("12345678900", "123456", "Gabriela Biondi");
        criarUsuarioPadrao("11122233344", "654321", "Investidor Teste");
        salvarUsuarios();
    }

    /* Cotações */
    f = fopen(ARQ_COTACOES, "r");
    if (!f || fscanf(f, "%lf %lf %lf", &cot.btc, &cot.eth, &cot.xrp) != 3) {
        cot.btc = 350000.00;
        cot.eth = 16000.00;
        cot.xrp = 2.50;
        salvarCotacoes();
    }
    if (f) fclose(f);
}

/* Cria o arquivo de extrato de um usuário (vazio) se ele ainda não existir.
 * O modo "a" não apaga o conteúdo de um extrato já existente. */
void criarArquivoExtrato(const char *cpf) {
    char nomeArq[40];
    FILE *f;

    sprintf(nomeArq, "extrato_%s.txt", cpf);
    f = fopen(nomeArq, "a");
    if (f) fclose(f);
    else printf("Erro ao criar %s!\n", nomeArq);
}

/* Cria todos os arquivos .txt automaticamente, se ainda não existirem */
void criarArquivosIniciais(void) {
    int i;

    /* usuarios.txt e cotacoes.txt (regrava com os dados atuais da memória) */
    salvarUsuarios();
    salvarCotacoes();

    /* extrato_<CPF>.txt de cada investidor */
    for (i = 0; i < totalUsuarios; i++)
        criarArquivoExtrato(usuarios[i].cpf);
}

/* Registra uma operação no extrato do usuário.
 * sinal: '+' ou '-'; valor: quantidade; simbolo: REAL/BTC/ETH/XRP
 * ct: cotação usada; tx: taxa aplicada */
void registrarExtrato(Usuario *u, char sinal, double valor, const char *simbolo,
                      double ct, double tx) {
    char nomeArq[40];
    char data[32];
    time_t agora = time(NULL);
    FILE *f;

    sprintf(nomeArq, "extrato_%s.txt", u->cpf);
    f = fopen(nomeArq, "a");
    if (!f) {
        printf("Erro ao gravar o extrato!\n");
        return;
    }
    strftime(data, sizeof(data), "%d-%m-%Y %H:%M", localtime(&agora));
    fprintf(f, "%s %c %.8g %s CT: %.2f TX: %.2f REAL: %.2f BTC: %.8g ETH: %.8g XRP: %.8g\n",
            data, sinal, valor, simbolo, ct, tx,
            u->reais, u->btc, u->eth, u->xrp);
    fclose(f);
}

/* ------------------------------------------------------------------ */
/* Autenticação e cadastro                                            */
/* ------------------------------------------------------------------ */

int buscarPorCPF(const char *cpf) {
    int i;
    for (i = 0; i < totalUsuarios; i++)
        if (strcmp(usuarios[i].cpf, cpf) == 0) return i;
    return -1;
}

/* Pede a senha de 6 dígitos para confirmar operações */
int confirmarSenha(Usuario *u) {
    char senha[32];
    printf("Digite sua senha (6 dígitos): ");
    lerLinha(senha, sizeof(senha));
    if (strcmp(senha, u->senha) != 0) {
        printf("Senha incorreta!\n");
        return 0;
    }
    return 1;
}

/* Retorna o índice do usuário logado ou -1 */
int efetuarLogin(void) {
    char cpf[32], senha[32];
    int idx;

    printf("\n===== LOGIN =====\n");
    printf("CPF (sem pontos e sem traços): ");
    lerLinha(cpf, sizeof(cpf));
    printf("Senha (6 dígitos): ");
    lerLinha(senha, sizeof(senha));

    if (!somenteDigitos(cpf, 11) || !somenteDigitos(senha, 6)) {
        printf("Formato inválido! O CPF deve ter 11 dígitos e a senha 6 dígitos numéricos.\n");
        return -1;
    }
    idx = buscarPorCPF(cpf);
    if (idx < 0 || strcmp(usuarios[idx].senha, senha) != 0) {
        printf("CPF ou senha incorretos!\n");
        return -1;
    }
    return idx;
}

/* Cadastra um novo investidor. Retorna o índice do novo usuário ou -1 */
int cadastrarUsuario(void) {
    char cpf[32], senha[32], nome[100];
    Usuario *u;
    int i, nomeValido = 0;

    printf("\n===== CADASTRO DE NOVO INVESTIDOR =====\n");

    if (totalUsuarios >= MAX_USUARIOS) {
        printf("Limite de usuários atingido!\n");
        return -1;
    }

    printf("CPF (11 dígitos, sem pontos e sem traços): ");
    lerLinha(cpf, sizeof(cpf));
    if (!somenteDigitos(cpf, 11)) {
        printf("CPF inválido! Digite exatamente 11 números.\n");
        return -1;
    }
    if (buscarPorCPF(cpf) >= 0) {
        printf("Já existe um investidor cadastrado com esse CPF!\n");
        return -1;
    }

    printf("Nome completo: ");
    lerLinha(nome, sizeof(nome));
    for (i = 0; nome[i]; i++)
        if (!isspace((unsigned char)nome[i])) nomeValido = 1;
    if (!nomeValido || strlen(nome) >= sizeof(usuarios[0].nome)) {
        printf("Nome inválido! Informe um nome com até 49 caracteres.\n");
        return -1;
    }
    if (strchr(nome, ';') != NULL) {
        printf("O nome não pode conter o caractere ';'.\n");
        return -1;
    }

    printf("Senha (6 dígitos numéricos): ");
    lerLinha(senha, sizeof(senha));
    if (!somenteDigitos(senha, 6)) {
        printf("Senha inválida! Digite exatamente 6 números.\n");
        return -1;
    }

    criarUsuarioPadrao(cpf, senha, nome);
    u = &usuarios[totalUsuarios - 1];
    (void)u;
    salvarUsuarios();
    criarArquivoExtrato(cpf);

    printf("Cadastro realizado com sucesso! Bem-vindo(a), %s!\n", nome);
    return totalUsuarios - 1;
}

/* Tela inicial: login, cadastro ou sair.
 * Retorna o índice do usuário logado, ou -1 se o usuário escolheu sair. */
int telaInicial(void) {
    int op, idx;

    do {
        printf("\n===== BEM-VINDO =====\n");
        printf("1. Fazer login\n");
        printf("2. Cadastrar novo investidor\n");
        printf("3. Sair\n");
        printf("Escolha: ");
        op = lerOpcao();

        switch (op) {
            case 1:
                idx = efetuarLogin();
                if (idx >= 0) return idx;
                break;
            case 2:
                idx = cadastrarUsuario();
                if (idx >= 0) return idx;   /* já entra logado após cadastrar */
                break;
            case 3:
                return -1;
            default:
                printf("Opção inválida!\n");
        }
    } while (1);
}

/* ------------------------------------------------------------------ */
/* Funcionalidades do menu                                            */
/* ------------------------------------------------------------------ */

void mostrarSaldos(Usuario *u) {
    char cpf[20], r[32], b[32], e[32], x[32];
    formatarCPF(u->cpf, cpf);
    fmt(r, u->reais, 2);
    fmt(b, u->btc, 8);
    fmt(e, u->eth, 8);
    fmt(x, u->xrp, 8);
    printf("\nNome: %s\n", u->nome);
    printf("CPF: %s\n", cpf);
    printf("Reais: %s\n", r);
    printf("Bitcoin: %s\n", b);
    printf("Ethereum: %s\n", e);
    printf("Ripple: %s\n", x);
}

void consultarSaldo(Usuario *u) {
    if (!confirmarSenha(u)) return;
    mostrarSaldos(u);
}

void consultarExtrato(Usuario *u) {
    char nomeArq[40], linha[300], cpf[20];
    FILE *f;
    int vazio = 1;

    if (!confirmarSenha(u)) return;

    formatarCPF(u->cpf, cpf);
    printf("\nNome: %s\n", u->nome);
    printf("CPF: %s\n", cpf);

    sprintf(nomeArq, "extrato_%s.txt", u->cpf);
    f = fopen(nomeArq, "r");
    if (!f) {
        printf("Nenhuma operação registrada.\n");
        return;
    }
    while (fgets(linha, sizeof(linha), f)) {
        printf("%s", linha);
        vazio = 0;
    }
    fclose(f);
    if (vazio) printf("Nenhuma operação registrada.\n");
}

void depositar(Usuario *u) {
    double v = lerValor("Valor do depósito (R$): ");
    if (v < 0) {
        printf("Valor inválido!\n");
        return;
    }
    u->reais += v;
    registrarExtrato(u, '+', v, "REAL", 0.0, 0.0);
    salvarUsuarios();
    printf("Depósito realizado com sucesso!\n");
    mostrarSaldos(u);
}

void sacar(Usuario *u) {
    double v = lerValor("Valor do saque (R$): ");
    if (v < 0) {
        printf("Valor inválido!\n");
        return;
    }
    if (v > u->reais) {
        printf("Saldo insuficiente! O saldo não pode ficar negativo.\n");
        return;
    }
    u->reais -= v;
    registrarExtrato(u, '-', v, "REAL", 0.0, 0.0);
    salvarUsuarios();
    printf("Saque realizado com sucesso!\n");
    mostrarSaldos(u);
}

void mostrarCotacoes(void) {
    char b[32], e[32], x[32];
    fmt(b, cot.btc, 2);
    fmt(e, cot.eth, 2);
    fmt(x, cot.xrp, 4);
    printf("\n--- Cotações atuais ---\n");
    printf("1. Bitcoin  (taxa compra 2%% / venda 3%%): R$ %s\n", b);
    printf("2. Ethereum (taxa compra 1%% / venda 2%%): R$ %s\n", e);
    printf("3. Ripple   (taxa compra 1%% / venda 1%%): R$ %s\n", x);
}

/* Obtém ponteiros para saldo, cotação, taxas e símbolo da moeda escolhida */
int dadosMoeda(int escolha, Usuario *u, double **saldo, double *ct,
               double *txCompra, double *txVenda, const char **simbolo) {
    switch (escolha) {
        case 1: *saldo = &u->btc; *ct = cot.btc; *txCompra = TX_COMPRA_BTC;
                *txVenda = TX_VENDA_BTC; *simbolo = "BTC"; return 1;
        case 2: *saldo = &u->eth; *ct = cot.eth; *txCompra = TX_COMPRA_ETH;
                *txVenda = TX_VENDA_ETH; *simbolo = "ETH"; return 1;
        case 3: *saldo = &u->xrp; *ct = cot.xrp; *txCompra = TX_COMPRA_XRP;
                *txVenda = TX_VENDA_XRP; *simbolo = "XRP"; return 1;
    }
    return 0;
}

void comprarCripto(Usuario *u) {
    double *saldo, ct, txC, txV, qtd, custo, taxa, total;
    const char *simbolo;
    int escolha;

    if (!confirmarSenha(u)) return;
    mostrarCotacoes();
    printf("Qual criptomoeda deseja comprar (1-3)? ");
    escolha = lerOpcao();
    if (!dadosMoeda(escolha, u, &saldo, &ct, &txC, &txV, &simbolo)) {
        printf("Opção inválida!\n");
        return;
    }

    qtd = lerValor("Quantidade que deseja comprar: ");
    if (qtd < 0) {
        printf("Quantidade inválida!\n");
        return;
    }

    custo = qtd * ct;
    taxa = custo * txC;
    total = custo + taxa;

    if (total > u->reais) {
        char t[32], s[32];
        fmt(t, total, 2);
        fmt(s, u->reais, 2);
        printf("Saldo insuficiente! Custo total: R$ %s (saldo: R$ %s)\n", t, s);
        return;
    }

    u->reais -= total;
    *saldo += qtd;
    registrarExtrato(u, '+', qtd, simbolo, ct, txC);
    salvarUsuarios();
    printf("Compra realizada! Custo: R$ %.2f + taxa de R$ %.2f = R$ %.2f\n",
           custo, taxa, total);
    mostrarSaldos(u);
}

void venderCripto(Usuario *u) {
    double *saldo, ct, txC, txV, qtd, bruto, taxa, liquido;
    const char *simbolo;
    int escolha;

    if (!confirmarSenha(u)) return;
    mostrarCotacoes();
    printf("Qual criptomoeda deseja vender (1-3)? ");
    escolha = lerOpcao();
    if (!dadosMoeda(escolha, u, &saldo, &ct, &txC, &txV, &simbolo)) {
        printf("Opção inválida!\n");
        return;
    }

    qtd = lerValor("Quantidade que deseja vender: ");
    if (qtd < 0) {
        printf("Quantidade inválida!\n");
        return;
    }
    if (qtd > *saldo) {
        printf("Saldo insuficiente de %s! O saldo não pode ficar negativo.\n", simbolo);
        return;
    }

    bruto = qtd * ct;
    taxa = bruto * txV;
    liquido = bruto - taxa;

    *saldo -= qtd;
    u->reais += liquido;
    registrarExtrato(u, '-', qtd, simbolo, ct, txV);
    salvarUsuarios();
    printf("Venda realizada! Bruto: R$ %.2f - taxa de R$ %.2f = R$ %.2f\n",
           bruto, taxa, liquido);
    mostrarSaldos(u);
}

/* Variação aleatória entre -5% e +5% */
double variar(double valor) {
    double fator = ((rand() % 10001) / 10000.0) * 0.10 - 0.05; /* -0.05 a +0.05 */
    return valor * (1.0 + fator);
}

void atualizarCotacao(void) {
    cot.btc = variar(cot.btc);
    cot.eth = variar(cot.eth);
    cot.xrp = variar(cot.xrp);
    salvarCotacoes();
    printf("Cotações atualizadas!\n");
    mostrarCotacoes();
}

/* ------------------------------------------------------------------ */
/* Programa principal                                                 */
/* ------------------------------------------------------------------ */

void menu(Usuario *u) {
    int op;
    do {
        printf("\n===== MENU - %s =====\n", u->nome);
        printf("1. Consultar saldo\n");
        printf("2. Consultar extrato\n");
        printf("3. Depositar\n");
        printf("4. Sacar\n");
        printf("5. Comprar criptomoedas\n");
        printf("6. Vender criptomoedas\n");
        printf("7. Atualizar cotação\n");
        printf("8. Sair\n");
        printf("Escolha: ");
        op = lerOpcao();

        switch (op) {
            case 1: consultarSaldo(u);   break;
            case 2: consultarExtrato(u); break;
            case 3: depositar(u);        break;
            case 4: sacar(u);            break;
            case 5: comprarCripto(u);    break;
            case 6: venderCripto(u);     break;
            case 7: atualizarCotacao();  break;
            case 8: printf("Saindo...\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (op != 8);
}

int main(void) {
    int idx;

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);   /* acentos corretos no console (arquivo em UTF-8) */
#endif

    srand((unsigned)time(NULL));
    carregarDados();
    criarArquivosIniciais();       /* cria usuarios.txt, cotacoes.txt e os extratos */

    printf("=== EXCHANGE DE CRIPTOMOEDAS ===\n");
    idx = telaInicial();
    if (idx < 0) {
        printf("Encerrando...\n");
        return 0;
    }

    menu(&usuarios[idx]);
    return 0;
}
