program test_semantic_undeclared_variable;
var a: integer;
begin
    /* Erro: 'x' não foi declarado */
    a := x;
end.
