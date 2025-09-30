program teste_erro_lexico;
var x : integer;
begin
    x := 10;
    @ /* caractere inválido que deve gerar erro */
    write(x);
end.