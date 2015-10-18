let
    var a: int;
    var i: int;
    var oszto: int;
    var vanoszto: bool;
in
    a = 15;
    vanoszto = false;
    i = a - 1;

    while (!vanoszto && i > 1) {
        if (a % i == 0) {
            vanoszto = true;
            oszto = i;
        }

        i = i - 1;
    }

    print(vanoszto);

    if (vanoszto) {
        print(oszto);
    }
end

