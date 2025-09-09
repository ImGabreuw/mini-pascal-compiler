# Mini Pascal Compiler

## Linguagem Mini Pascal

- Linguagem case sensitive
- Tipos de dados suportados:
  - Numeral (`integer`): `3`, `-7`
  - Booleanos (`boolean`): `True` e `False`
- Literais (strings) apenas para mensagens de saída
  - NÃO PODE:
    - Declarar / atribuir strings a variáveis
    - Operações com strings (comparação, concatenação, etc)

Exemplo de código em Mini Pascal:

```pascal
program correto ;
var a, b, c : integer ;
var d, e, f : boolean ;
procedure proc(var a1 : integer ) ;
var a, b, c : integer ;
var d, e, f : boolean ;
begin
    a := 1 ;
    if ( a < 1 ) then
        a := 12 ;
end ;
begin
    a := 2 ;
    b := 10 ;
    c := 11 ;
    a := b + c ;
    d := true ;
    e := false ;
    f := true ;
    write ( b ) ;
    if ( d ) then
        begin
            a := 20 ;
            b := 10 * c ;
            c := a div b ;
        end ;
    while ( a > 1 ) do
        begin
            if ( b > 10 ) then
                b := 2 ;
            a := a - 1 ;
        end ;
end .
```

### Gramática

- Comentários: `/* ... */`
- Identificadores (variáveis)
  - Iniciam com letra ou `_`
  - Seguidos por 0 ou mais letras, dígitos ou `_`
- Tipos de dados
  - `integer`
  - `boolean` (`True`, `False`)
- Operadores
  - Aritméticos: `+`, `-`, `*`, `div` (divisão inteira)
  - Relacionais: `<`, `>`, `<>` (diferente), `<=`, `>=`, `:=` (atribuição)
  - Lógicos: `and`, `or`, `not`
- Delimitadores
  - `;` (fim de comando)
  - `,` (separador de variáveis)
  - `.` (fim do programa)
- `(`, `)` (agrupamento)
- Palavras reservadas:
    - `program`, `begin`, `end`, `procedure`
    - `if`, `then`, `else`, `while`, `do`
    - `and`, `or`, `not`
    - `var`, `integer`, `boolean`, `true`, `false`
    - `write`