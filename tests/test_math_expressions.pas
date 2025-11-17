program test_math_expressions;
var
    a, b, c, d: integer;
begin
    a := 10;
    b := 5;
    c := 2;

    /* Expressão com múltiplas operações aritméticas e parênteses */
    d := (a + b) * c - (a div b);
    
    /* Esperado: (10 + 5) * 2 - (10 div 5) = 15 * 2 - 2 = 30 - 2 = 28 */
    write(d);

    /* Expressão com sinal negativo */
    a := -10;
    b := a * 2;
    
    /* Esperado: -10 * 2 = -20 */
    write(b);
end.
