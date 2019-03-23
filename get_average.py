import os

def main():
    input_directory = 'output/'
    output_directory = 'averages/'

    filenames = os.listdir(input_directory)

    for filename in filenames:
        input_file = open(input_directory + filename, 'r')
        lines = input_file.readlines()
        input_file.close()

        count = len(lines)
        lines_sum = sum(float(line[:-1]) for line in lines)
        average = lines_sum / count

        output_file = open(output_directory + filename, 'w+')
        output_file.write('{:.{prec}f}\n'.format(average, prec = 8))
        output_file.close()

main()
