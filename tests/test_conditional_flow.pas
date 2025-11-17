program test_conditional_flow;
var a: integer;
begin
    a := 20;

    if a > 10 then
        /* Esperado: 1 */
        write(a)
    else
        write(a);

    if a < 10 then
        write(a)
    else
        /* Esperado: 0 */
        write(a);
end.
