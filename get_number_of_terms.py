import math
import os

def calc_sh_term(n, x):
    odd_n = 2 * n + 1
    term =  x ** odd_n / math.factorial(odd_n)

    return term

def calc_terms_number(x):
    e = 0.00000001

    term = float('inf')
    n = 0

    while (math.fabs(term) >= e):
        term = calc_sh_term(n, x)
        n += 1

    return n

def main():
    input_directory = 'input/'
    output_directory = 'terms_number/'

    filenames = os.listdir(input_directory)

    for filename in filenames:
        input_file = open(input_directory + filename, 'r')
        lines = input_file.readlines()
        input_file.close()

        new_lines = [calc_terms_number(float(line[:-1])) for line in lines]
        formatted_lines = ['{:d}\n'.format(line) for line in new_lines]

        output_file = open(output_directory + filename, 'w+')
        output_file.writelines(formatted_lines)
        output_file.close()

main()
