# Relatório de Análise Crítica com IA
**Trabalho Final — Programação Concorrente (C / C++)**
**Tema A — Servidor de Chat Multiusuário (TCP)**

---

## Objetivo

Este relatório apresenta a análise crítica de concorrência do projeto Servidor de Chat Multiusuário TCP, desenvolvida com o auxílio de uma IA (LLM - ChatGPT), conforme exigido na Etapa 3 do trabalho.

O objetivo foi identificar possíveis problemas de concorrência (race conditions, deadlocks, starvation, etc.) e demonstrar como o projeto os mitiga através do uso correto de threads, exclusão mútua, semáforos, variáveis de condição e monitores.

---

## Descrição Geral do Sistema

O sistema implementa um chat multiusuário em rede (TCP) composto por:

- Servidor (ChatServer):
  - Aceita múltiplas conexões simultâneas.
  - Cada cliente é atendido por uma thread dedicada.
  - Mensagens recebidas são retransmitidas (broadcast) a todos os demais.
  - Histórico e lista de clientes são protegidos por mutexes.
  - Logging concorrente é feito via TSLogger.

- Cliente (ChatClient):
  - Conecta ao servidor, registra o nome e inicia threads de envio e recepção.
  - Interage via CLI (linha de comando).

- Bibliotecas de suporte:
  - TSLogger: logging thread-safe.
  - ThreadSafeQueue: fila protegida por mutex e condition_variable.
  - Semaphore: semáforo genérico para controle de sincronização (opcional).

---

## Análise de Concorrência (com IA)

A seguir estão as principais situações concorrentes identificadas no sistema e a análise realizada com o auxílio de uma IA (ChatGPT).

### 1. Concorrência no acesso à lista de clientes (clients)

Risco identificado: Race condition ao adicionar ou remover clientes conectados.

Código relevante:
```cpp
std::vector<int> clients;
std::mutex clients_mutex;
```

Análise da IA:
Cada thread que trata um cliente pode adicionar ou remover entradas na lista clients.  
O uso de std::lock_guard<std::mutex> em todas as operações garante exclusão mútua, evitando inconsistência no vetor e no mapa client_names.

Mitigação:  
- Uso de std::lock_guard<std::mutex> nas seções críticas.  
- Acesso controlado também no broadcast (dispatcher_loop), evitando acesso simultâneo.

---

### 2. Concorrência no histórico de mensagens (history)

Risco identificado: Duas threads podem tentar adicionar mensagens ao histórico ao mesmo tempo.

Código relevante:
```cpp
std::vector<std::string> history;
std::mutex history_mutex;
```

Análise da IA:
O uso do history_mutex ao redor da função add_history() impede que múltiplas threads modifiquem o histórico simultaneamente, evitando corrupção de memória ou escrita intercalada.

Mitigação:  
- Mutex dedicado ao histórico.  
- Histórico também é lido de forma protegida ao enviar mensagens antigas ao novo cliente.

---

### 3. Concorrência no logging (TSLogger)

Risco identificado: Múltiplas threads gravando no mesmo arquivo de log simultaneamente.

Código relevante:
```cpp
void TSLogger::log(const std::string &msg) {
    std::lock_guard<std::mutex> lock(log_mutex);
    logfile << "[" << timestamp() << "] " << msg << std::endl;
}
```

Análise da IA:
O logger é thread-safe, garantindo que cada chamada a log() é atômica.  
Não há risco de interleaving ou perda de mensagens.

Mitigação:  
- Mutex interno log_mutex protege o arquivo.  
- Teste de estresse (main_logger_test.cpp) confirma integridade do log sob 5 threads concorrentes.

---

### 4. Concorrência entre produtor/consumidor (ThreadSafeQueue)

Risco identificado: Threads produtoras (clientes) e consumidora (dispatcher) acessam a mesma fila simultaneamente.

Código relevante:
```cpp
std::condition_variable cv;
std::mutex m;
std::queue<T> q;
```

Análise da IA:
O monitor ThreadSafeQueue encapsula a sincronização com mutex e condition_variable.  
O método pop() aguarda mensagens novas de forma eficiente (sem busy waiting).  
O fechamento da fila com close() evita deadlocks quando o servidor é encerrado.

Mitigação:  
- Encapsulamento monitor-style.  
- Bloqueio seguro e notificação controlada.  
- Tratamento de fim de fila (std::optional<T>).

---

### 5. Possível deadlock entre clients_mutex e history_mutex

Risco identificado: Deadlock se duas threads adquirirem locks em ordem inversa.

Análise da IA:
Nenhuma função bloqueia mais de um mutex por vez.  
Locks são curtos e sempre liberados imediatamente.  
Não há chamadas encadeadas entre funções que bloqueiem múltiplos mutexes.

Mitigação:  
- Locks locais e independentes.  
- Nenhum lock aninhado.  
- Uso de RAII (std::lock_guard) garante liberação automática.

---

### 6. Encerramento seguro do servidor (stop() e dispatcher_loop())

Risco identificado: Thread de dispatcher_loop pode ficar bloqueada indefinidamente em pop().

Análise da IA:
O método ThreadSafeQueue::close() notifica todas as threads bloqueadas.  
pop() retorna std::nullopt e a thread do dispatcher encerra normalmente.

Mitigação:  
- message_queue.close() ao encerrar o servidor.  
- Tratamento correto no loop do dispatcher.

---

## Recursos de Concorrência Utilizados

| Recurso | Implementação | Propósito |
|----------|----------------|------------|
| Threads (std::thread) | Servidor e clientes concorrentes | Atender conexões simultâneas |
| Mutex (std::mutex) | Proteção de logs, lista e histórico | Exclusão mútua |
| Condition Variable | ThreadSafeQueue | Sincronização produtor/consumidor |
| Semáforo (Semaphore) | Implementado em tsqueue.hpp | Recurso adicional (não utilizado diretamente) |
| Monitor | ThreadSafeQueue | Encapsula sincronização |
| Sockets TCP | socket(), accept(), recv(), send() | Comunicação entre processos |

---

## Conclusão da Análise

A IA confirmou que o sistema:
- Não apresenta race conditions conhecidas.
- Evita deadlocks pelo uso simples e isolado de mutexes.
- Garante integridade de logs e mensagens.
- Trata corretamente o encerramento das threads e sockets.
- Encapsula bem a sincronização via monitores e RAII.

Em termos de robustez concorrente, o sistema atende plenamente aos requisitos de sincronização, exclusão mútua e thread safety propostos na disciplina.

---

## Prompts Utilizados (Interação com a IA)

Trechos de prompts usados para a análise:

> "Analise se este código C++ de chat multiusuário possui riscos de race condition ou deadlock."
> 
> "Explique se o uso de mutexes em ChatServer garante exclusão mútua entre threads."
> 
> "Avalie se a classe ThreadSafeQueue implementa corretamente o padrão de monitor."
> 
> "Liste possíveis cenários de deadlock no servidor e como preveni-los."

A IA (ChatGPT, modelo GPT-5) foi utilizada apenas para análise e revisão de concorrência, sem interferir na lógica principal do código.

---

## Referências

- Material da disciplina “Programação Concorrente (LPII - 2025.1)” – Prof. Augusto de Holanda, UFPB.
- Documentação C++17 (std::thread, std::mutex, std::condition_variable).
- Recomendações POSIX para sockets TCP.
- Saída de testes obtida via tests/run_test.sh.

---

Conclusão Final:  
O projeto do Tema A — Servidor de Chat Multiusuário TCP — cumpre integralmente os requisitos técnicos e demonstra domínio prático dos conceitos de programação concorrente, sincronização de threads e comunicação em rede.