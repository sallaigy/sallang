let
    var a: int;
    var i: int;
    var oszto: int;
    var vanoszto: bool;
in
    a = 15;
    vanoszto = false;
    i = 2;

    while (!vanoszto && i < a) {
        if (a % i == 0) {
            vanoszto = true;
            oszto = i;
        }

        i = i + 1;
    }

    print(vanoszto);

    if (vanoszto) {
        print(oszto);
    }
end

