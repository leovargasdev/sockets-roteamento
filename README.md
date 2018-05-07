## Simulador do protocolo de roteamento, via sockets UDP ###

### Colaboradores ###

- Leonardo Vargas
- Marcelo Acordi


### Configurações de diretório ###
No mesmo diretório do arquivo **main.c**, deve conter uma pasta input/ e nela deve ter dois arquivos:

- **enlaces.config**
    - Arquivo responsável por conter os enlaces entre os roteadores, em cada linha contém dois inteiros que representa dois id's dos roteadores
    e logo em seguida o peso do enlace.

- **roteador.config**
    - Arquivo responsável por conter a configuração dos roteadores, em cada linha contém um inteiro que representa o id do roteador, por segundo
    a porta referente a entrada e saida de dados do roteador e por último o ip da maquina que utilizará o roteador.


### Compilação e execução ###
Deve-se utilizadar o seguinte comando para compilar e executar o código:
    $ gcc main.c -o main -lpthread
    $ ./main

## Como utilizar ##
Após ter tido sucesso no passo de compilação, a primeira entrada espereada pelo código é o roteador origem, no caso o roteador
que o usuário deseja instânciar.
Com o roteador selecionado deve-se dizer qual o roteador destino do pacote.
**obs**: Caso o roteador destino ou os roteadores que fazem a ligação até o distino não forem instânciados, esse pacote
nunca será entregue. Pois este algoritmo não se ajusta conforme o cenário, ele leva enconta o caminho gerado no dijstra
a partir do arquivo **enlaces.config**
O próximo e último passo é escrever a mensagem, está mensagem deve conter no máximo 100 caracteres.
Se a mensagem for entregue com sucesso aparecerá um mensagem de confirmação de entrega, e após isso pode-se selecionar
ouro roteador destino para entrega de outro pacote.
