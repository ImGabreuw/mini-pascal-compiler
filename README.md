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

### Elementos da linguagem

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

## Gramática Livre de Contexto (GLC)

### Estrutura do Programa

$$\langle program \rangle ::= \text{program } \langle identifier \rangle \text{ ; } \langle block \rangle \text{ .}$$

$$\langle block \rangle ::= \langle variable\ declaration\ part \rangle \langle subroutine\ declaration\ part \rangle \langle statement\ part \rangle$$

### Declarações

$$\langle variable\ declaration\ part \rangle ::= \langle empty \rangle \mid \text{var } \langle variable\ declaration \rangle \text{ ; } \{ \langle variable\ declaration\ part \rangle \text{ ; } \}$$

$$\langle variable\ declaration \rangle ::= \langle identifier \rangle \{ \text{ , } \langle identifier \rangle \} \text{ : } \langle type \rangle$$

$$\langle type \rangle ::= \text{integer} \mid \text{boolean}$$

$$\langle subroutine\ declaration\ part \rangle ::= \langle procedure\ declaration \mid function\ declaration \rangle$$

$$\langle procedure\ declaration \rangle ::= \text{procedure } \langle identifier \rangle \langle formal\ parameters \rangle \text{ ; } \langle block \rangle$$

$$\langle function\ declaration \rangle ::= \text{function } \langle identifier \rangle \langle formal\ parameters \rangle \text{ : } \langle type \rangle \text{ ; } \langle block \rangle$$

$$\langle formal\ parameters \rangle ::= \langle variable\ declaration\ part \rangle$$

> **OBS:** Todos os identificadores devem ser declarados antes de serem utilizados.

### Comandos

$$\langle statement\ part \rangle ::= \text{begin } \langle statement \rangle \{ \text{ ; } \langle statement \rangle \} \text{ end}$$

$$\langle statement \rangle ::= \langle assignment\ statement \rangle$$

$$\langle statement \rangle ::= \langle function\_procedure\ statement \rangle$$

$$\langle statement \rangle ::= \langle read\ statement \rangle$$

$$\langle statement \rangle ::= \langle write\ statement \rangle$$

$$\langle statement \rangle ::= \langle compound\ statement \rangle$$

$$\langle statement \rangle ::= \langle if\ statement \rangle$$

$$\langle statement \rangle ::= \langle while\ statement \rangle$$

$$\langle assignment\ statement \rangle ::= \langle variable \rangle \text{ := } \langle expression \rangle$$

$$\langle function\_procedure\ statement \rangle ::= \langle function\_procedure\ identifier \rangle \text{ ( parameters list ) }$$

$$\langle function\_procedure\ statement \rangle ::= \langle variable \rangle \text{ := } \langle function\_procedure\ identifier \rangle \text{ ( parameters list )}$$

$$\langle function\_procedure\ identifier \rangle ::= \langle identifier \rangle$$

$$\langle parameters\ list \rangle ::= \text{( } (\langle identifier \rangle \mid \langle number \rangle \mid \langle bool \rangle) \{ \text{, } (\langle identifier \rangle \mid \langle number \rangle \mid \langle bool \rangle) \} \text{ )}$$

$$\langle read\ statement \rangle ::= \text{read ( } \langle variable \rangle \{ \text{ , } \langle variable \rangle \} \text{ )}$$

$$\langle write\ statement \rangle ::= \text{write ( } \langle variable \rangle \{ \text{ , } \langle variable \rangle \} \text{ )}$$

$$\langle if\ statement \rangle ::= \text{if } \langle expression \rangle \text{ then } \langle statement \rangle \{ \text{ else } \langle statement \rangle \}$$

$$\langle while\ statement \rangle ::= \text{while } \langle expression \rangle \text{ do } \langle statement \rangle$$

### Expressões

$$\langle expression \rangle ::= \langle simple\ expression \rangle$$

$$\langle expression \rangle ::= \langle simple\ expression \rangle \langle relational\ operator \rangle \langle simple\ expression \rangle$$

$$\langle simple\ expression \rangle ::= \langle sign \rangle \langle term \rangle \{ \langle adding\ operator \rangle \langle term \rangle \}$$

$$\langle term \rangle ::= \langle factor \rangle \{ \langle multiplying\ operator \rangle \langle factor \rangle \}$$

$$\langle factor \rangle ::= \langle variable \rangle$$

$$\langle factor \rangle ::= \langle constant \rangle$$

$$\langle factor \rangle ::= \text{( } \langle expression \rangle \text{ )}$$

$$\langle factor \rangle ::= \text{not } \langle factor \rangle$$

$$\langle factor \rangle ::= \langle bool \rangle$$

$$\langle relational\ operator \rangle ::= \text{=} \mid \text{<>} \mid \text{<} \mid \text{<=} \mid \text{>=} \mid \text{>} \mid \text{or} \mid \text{and}$$

$$\langle sign \rangle ::= \text{+} \mid \text{-} \mid \langle empty \rangle$$

$$\langle adding\ operator \rangle ::= \text{+} \mid \text{-}$$

$$\langle multiplying\ operator \rangle ::= \text{*} \mid \text{div}$$

$$\langle variable \rangle ::= \langle identifier \rangle$$

### Números e Identificadores

$$\langle constant \rangle ::= \langle integer\ constant \rangle \mid \langle constant\ identifier \rangle$$

$$\langle constant\ identifier \rangle ::= \langle identifier \rangle$$

$$\langle identifier \rangle ::= \langle letter \rangle \{ \langle letter \rangle \mid \langle digit \rangle \}$$

$$\langle integer\ constant \rangle ::= \langle digit \rangle \{ \langle digit \rangle \}$$

$$\langle bool \rangle ::= \text{true} \mid \text{false}$$

$$\langle letter \rangle ::= \text{a} \mid \text{b} \mid \text{c} \mid \text{d} \mid \text{e} \mid \text{f} \mid \text{g} \mid \text{h} \mid \text{i} \mid \text{j} \mid \text{k} \mid \text{l} \mid \text{m} \mid \text{n} \mid \text{o}$$

$$\langle letter \rangle ::= \text{p} \mid \text{q} \mid \text{r} \mid \text{s} \mid \text{t} \mid \text{u} \mid \text{v} \mid \text{w} \mid \text{x} \mid \text{y} \mid \text{z}$$

$$\langle letter \rangle ::= \text{A} \mid \text{B} \mid \text{C} \mid \text{D} \mid \text{E} \mid \text{F} \mid \text{G} \mid \text{H} \mid \text{I} \mid \text{J} \mid \text{K} \mid \text{L} \mid \text{M} \mid \text{N} \mid \text{O}$$

$$\langle letter \rangle ::= \text{P} \mid \text{Q} \mid \text{R} \mid \text{S} \mid \text{T} \mid \text{U} \mid \text{V} \mid \text{W} \mid \text{X} \mid \text{Y} \mid \text{Z}$$

$$\langle digit \rangle ::= \text{0} \mid \text{1} \mid \text{2} \mid \text{3} \mid \text{4} \mid \text{5} \mid \text{6} \mid \text{7} \mid \text{8} \mid \text{9}$$

$$\langle special\ symbol \rangle ::= \text{+} \mid \text{-} \mid \text{*} \mid \text{=} \mid \text{<>} \mid \text{<} \mid \text{>} \mid \text{<=} \mid \text{>=}$$

$$\langle special\ symbol \rangle ::= \text{(} \mid \text{)} \mid \text{[} \mid \text{]} \mid \text{:=} \mid \text{.} \mid \text{,} \mid \text{;} \mid \text{:} \mid \text{..}$$

$$\langle special\ symbol \rangle ::= \text{div} \mid \text{or} \mid \text{and} \mid \text{not} \mid \text{if} \mid \text{then} \mid \text{else} \mid \text{of}$$

$$\langle special\ symbol \rangle ::= \text{while} \mid \text{do} \mid \text{begin} \mid \text{end} \mid \text{read} \mid \text{write} \mid \text{var}$$

$$\langle special\ symbol \rangle ::= \text{array} \mid \text{function} \mid \text{procedure} \mid \text{program} \mid \text{true} \mid \text{false} \mid \text{char} \mid \text{integer} \mid \text{boolean}$$

### Notação EBNF

- $\{α\}$ = repetição da cadeia $α$ zero ou mais vezes
- $α \mid β$ = $α$ ou $β$ devem ser escolhidos
- Não terminais aparecem entre $\langle$ e $\rangle$
