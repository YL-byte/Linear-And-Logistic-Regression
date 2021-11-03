#include <iostream>
#include <stdexcept>
#include "..//neuralnetworkmodel.cpp"
#include "..//Additional Libraries//file_functions.cpp"
#define INPUT_SHAPE 784
#define OUTPUT_SHAPE 10
#define MAX_PIXEL 256

int main(){
    int nn_shape[] = {INPUT_SHAPE, OUTPUT_SHAPE};
    int nn_size = sizeof(nn_shape) / sizeof(int);
    NeuralNetwork nn(nn_shape, nn_size, 0.01, true);
    int number_of_samples;
    printf("Load weights and biases.\n");
    float *weights_raw = read_table("..//data//weights.txt", " ,", &number_of_samples);
    float *biases_raw = read_table("..//data//biases.txt", " ,", &number_of_samples);
    nn.weights[0] = weights_raw;
    nn.biases[0] = biases_raw;
    int success = 0;
    int success_hist[10] = {0};
    int failure = 0;
    int failure_hist[10] = {0};
    float *testing_data_raw = read_table("..//Data//MNIST_test.txt", " ,", &number_of_samples);
    Matrix testing_data_matrix(number_of_samples, 1);
    testing_data_matrix = testing_data_raw;
    printf("Loaded Testing Samples And Test Module:\n");
    for (int i = 0; i < number_of_samples / (INPUT_SHAPE + 1); i++){
            Matrix input(INPUT_SHAPE, 1);
            int current_start_index = i * INPUT_SHAPE;
            int output_index = (int)testing_data_raw[current_start_index];
            input = testing_data_matrix.subMatrix(current_start_index + 1, current_start_index + 1 + INPUT_SHAPE, 0, 1);
            for (int row_index = 0; row_index < input.rows; row_index++){
//                if(input.matrix[row_index][0] != 0){
//                    input.matrix[row_index][0] = 1;
//                }
                input.matrix[row_index][0] /= MAX_PIXEL;
            }
            Matrix expected_output(10, 1);
            expected_output.populateWithValue_(0);
            expected_output.matrix[output_index][0] = 1;
            Matrix output = nn.forward(input);
            int predicted_output_index = 0;
            float predicted_output_value = output.matrix[0][0];
            bool is_success;
            for (int i = 0; i < 10; i++){
                if (predicted_output_value < output.matrix[i][0]){
                    predicted_output_value = output.matrix[i][0];
                    predicted_output_index = i;
                }
            }
            for (int i = 0; i < 10; i++){
                if (i == predicted_output_index){
                    output.matrix[i][0] == 1;
                }
                else {
                    output.matrix[i][0] == 0;
                }
            }
            if (predicted_output_index == output_index){
                success++;
                success_hist[output_index]++;
                is_success = true;
            }
            else{
                failure++;
                failure_hist[output_index]++;
                is_success = false;
            }
            printf("Current Expected Output: %d | Output: %d\n", output_index, predicted_output_index);
            printf("Expected Output: ");
            expected_output.transpose_();
            expected_output.printMatrix();
            printf("Output: ");
            output.transpose_();
            output.printMatrix();
            printf("Is Success: %d\n", is_success);
            printf("Success Rate: %f\n", (float)success / (success + failure));
            printf("___________\n");
            output.freeMatrix_();
            input.freeMatrix_();
            expected_output.freeMatrix_();
    }
    printf("Successes: %d | Failures: %d\n", success, failure);
    for (int i = 0; i < 10; i++){
        printf("For Letter %d: Success: %d | Failure: %d\n", i, success_hist[i], failure_hist[i]);
    }
    free(weights_raw);
    free(biases_raw);
    testing_data_matrix.freeMatrix_();
    nn.freeAllMatrices_();
}
