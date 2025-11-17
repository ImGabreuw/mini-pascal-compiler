program test_while_loop;
var counter: integer;
begin
    counter := 0;
    while counter < 5 do
    begin
        write(counter);
        counter := counter + 1;
    end;
    /* Esperado: imprimir 0, 1, 2, 3, 4 */
end.
