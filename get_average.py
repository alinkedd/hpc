import os

def main():
    input_directory = 'output/'
    output_directory = 'averages/'
    number_of_iterations = 1000.0

    filenames = os.listdir(input_directory)

    for filename in filenames:
        input_file = open(input_directory + filename, 'r')
        line = input_file.readline()
        input_file.close()

        average = float(line[:-1]) / number_of_iterations

        output_file = open(output_directory + filename, 'w+')
        output_file.write('{:.{prec}f}\n'.format(average, prec = 10))
        output_file.close()

main()
