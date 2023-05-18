





even(int *list, int len) { int sum = 0; for (int i = 0; i < len; i++) { if (list[i] % 2 == 0) { sum += list[i]; } } return sum; } 


if reverse_words('Hello world') == 'olleH dlrow':
    if reverse_words('This is a test') == 'sihT si a tset':
        if reverse_words('The quick brown fox jumps over the lazy dog') == 'ehT kciuq nworb xof spmuj revo eht yzal god':
            if reverse_words('ab cd ef gh ij kl mn') == 'ba dc fe hg ji lk nm':
                if reverse_words('Python is a popular programming language') == 'nohtyP si a ralupop gnimmargorp egaugnal':
                    print("0")
                else:
                    print("Failed test case 5")
            else:
                print("Failed test case 4")
        else:
            print("Failed test case 3")
    else:
        print("Failed test case 2")
else:
    print("Failed test case 1")