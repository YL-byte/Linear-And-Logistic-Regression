#include <iostream>
#include <stdexcept>
#include "..//neuralnetworkmodel.cpp"
#include "..//Additional Libraries//file_functions.cpp"
#define INPUT_SHAPE 784
#define OUTPUT_SHAPE 10
#define MAX_PIXEL 256

int main(){
    int number_of_samples;
    float *training_data_raw = read_table("..//Data//MNIST_train.txt", ",", &number_of_samples);
    Matrix training_data_matrix(number_of_samples, 1);
    training_data_matrix = training_data_raw;
    printf("Loaded Samples And Start Training:\n");

    int nn_shape[] = {INPUT_SHAPE, OUTPUT_SHAPE};
    int nn_size = sizeof(nn_shape) / sizeof(int);
    NeuralNetwork nn(nn_shape, nn_size, 0.0001, true);
    int success = 0;
    int failure = 0;
    for (int i = 0; i < number_of_samples / (INPUT_SHAPE + 1); i++){
        Matrix input(INPUT_SHAPE, 1);
        int current_start_index = i * INPUT_SHAPE;
        int output_index = (int)training_data_raw[current_start_index];
        input = training_data_matrix.subMatrix(current_start_index + 1, current_start_index + 1 + INPUT_SHAPE, 0, 1);
        for (int row_index = 0; row_index < input.rows; row_index++){
            if(input.matrix[row_index][0] != 0){
                input.matrix[row_index][0] = 1;
            }
//            input.matrix[row_index][0] /= MAX_PIXEL;
        }
        Matrix expected_output(10, 1);
        expected_output.populateWithValue_(0);
        expected_output.matrix[output_index][0] = 1;
        float l;
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
            is_success = true;
        }
        else{
            failure++;
            is_success = false;
        }
        nn.train_(input, expected_output, &l);
//        printf("Current Expected Output: %d | Output: %d\n", output_index, predicted_output_index);
//        printf("Loss: %f\n", l);
//        printf("Expected Output: ");
//        expected_output.transpose_();
//        expected_output.printMatrix();
//        printf("Output: ");
//        output.transpose_();
//        output.printMatrix();
//        printf("Is Success: %d\n", is_success);
//        printf("Success Rate: %f\n", (float)success / (success + failure));
//        printf("___________\n");
        output.freeMatrix_();
        input.freeMatrix_();
        expected_output.freeMatrix_();
    }

    printf("Successes: %d | Failures: %d\n", success, failure);
    success = 0;
    failure = 0;
    printf("Writing Weights And Biases To Txt Files.\n");
    File weights_file, biases_file;
    weights_file = "..//Data//weights.txt";
    weights_file.write("");
    biases_file = "..//Data//biases.txt";
    biases_file.write("");
    //Save Weights
    for (int layer_index = 0; layer_index < nn.number_of_layers - 1; layer_index++){
        for (int row_index = 0; row_index < nn.shape[layer_index + 1]; row_index++){
            for (int column_index = 0; column_index < nn.shape[layer_index]; column_index++){
                char float_char[15];
                sprintf(float_char, "%.15f", nn.weights[layer_index].matrix[row_index][column_index]);
                weights_file.appendFromPointer(float_char);
                weights_file.appendCharacter(' ');
            }
            weights_file.appendCharacter('\n');
            weights_file.appendCharacter('\n');
        }
    }
    //Save Biases
    for (int layer_index = 0; layer_index < nn.number_of_layers; layer_index++){
        for (int node_index = 0; node_index < nn.shape[layer_index]; node_index++){
            char float_char[15];
            sprintf(float_char, "%.15f", nn.biases[layer_index].matrix[node_index][0]);
            biases_file.appendFromPointer(float_char);
            biases_file.appendCharacter(' ');
        }
        biases_file.appendCharacter('\n');
        biases_file.appendCharacter('\n');
    }
    weights_file.freeFile_();
    biases_file.freeFile_();
    nn.freeAllMatrices_();

    free(training_data_raw);
    training_data_matrix.freeMatrix_();
}
