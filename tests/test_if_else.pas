program test_if_else;
var a, b, c : integer;
begin
    a := 5;
    b := 10;

    if a < b then
        c := a + b
    else
        c := a - b;

    write(a, b, c)
end.