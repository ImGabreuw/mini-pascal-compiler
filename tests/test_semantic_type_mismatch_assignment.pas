program test_semantic_type_mismatch_assignment;
var age: integer;
var is_adult: boolean;
begin
    /* Erro: não pode atribuir boolean para integer */
    age := true;
    /* Erro: não pode atribuir integer para boolean */
    is_adult := 25;
end.
