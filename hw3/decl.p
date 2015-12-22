//&S-;
//&T-;
test;

// no global declaration(s)

func( a:integer;b:array 1 to 5 of array 2 to 4 of array 5 to 10 of real ): boolean;
begin
    var c: "hello world!";
    var z:6;
    var j:6.8;
    var o:false;
    begin
        var d: real;
        return (b[1][4] >= 1.0);
    end
end
end func

begin
    var a: real;
    begin
        var a: boolean; // outer 'a' has been hidden in this scope
    end
end
end test1
