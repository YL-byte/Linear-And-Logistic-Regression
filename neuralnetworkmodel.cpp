#include "..//Matrix Operations//matrix.cpp"
#include <cmath>
#include<stdlib.h>

float sigmoid(float x);
float sigmoid_d(float x);
class NeuralNetwork{
    public:
        int number_of_layers;
        int *shape;
        float learning_rate;
        Matrix *weights = NULL; //Weights Indexing Method: [from_layer][to_node_in_upper_layer][from_node_in_lower_layer]
        Matrix *layers = NULL;
        Matrix *biases = NULL;
        Matrix *weights_gradient = NULL;
        Matrix *layers_gradient = NULL;
        Matrix *biases_gradient = NULL;
        bool logistic_regression;
        NeuralNetwork(int Arr_shape[], int Arr_number_of_layers, float Arr_learning_rate = 0.01, bool Arr_logistic_regression = false){
            number_of_layers = Arr_number_of_layers;
            shape = Arr_shape;
            learning_rate = Arr_learning_rate;
            logistic_regression = Arr_logistic_regression;
            layers = (Matrix *)malloc(sizeof(Matrix) * number_of_layers);
            weights = (Matrix *)malloc(sizeof(Matrix) * (number_of_layers - 1));
            biases = (Matrix *)malloc(sizeof(Matrix) * number_of_layers);
            layers_gradient = (Matrix *)malloc(sizeof(Matrix) * number_of_layers);
            weights_gradient = (Matrix *)malloc(sizeof(Matrix) * (number_of_layers - 1));
            biases_gradient = (Matrix *)malloc(sizeof(Matrix) * number_of_layers);
            //Create Layers And Initialize Them With 0
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                layers[layer_index] = Matrix(shape[layer_index], 1);
                layers[layer_index].populateWithValue_(0);
                layers_gradient[layer_index] = Matrix(shape[layer_index], 1);
                layers_gradient[layer_index].populateWithValue_(0);
            }
            //Create Weights And Initialize Them With 0
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                weights[layer_index] = Matrix(shape[layer_index + 1], shape[layer_index]);
                weights[layer_index].populateWithValue_(0.1);
                weights_gradient[layer_index] = Matrix(shape[layer_index + 1], shape[layer_index]);
                weights_gradient[layer_index].populateWithValue_(0);
            }
            //Create Biases And Initialize Them With 0
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                biases[layer_index] = Matrix(shape[layer_index], 1);
                biases[layer_index].populateWithValue_();
                biases_gradient[layer_index] = Matrix(shape[layer_index], 1);
                biases_gradient[layer_index].populateWithValue_(0);
            }
        }

        Matrix forward(Matrix input){
            check_if_vector_is_valid(input, 0);
            //Assign the input to the first layer
            layers[0] = input.copyMatrix() + biases[0];
            for (int layer_index = 1; layer_index < number_of_layers; layer_index++){
                layers[layer_index].freeMatrix_();
                layers[layer_index] = weights[layer_index - 1] * layers[layer_index - 1] + biases[layer_index];

                if (logistic_regression){
                    for (int row_index = 0; row_index < layers[layer_index].rows; row_index++){
                        layers[layer_index].matrix[row_index][0] = sigmoid(layers[layer_index].matrix[row_index][0]);
                    }
                }

            }
            Matrix output = layers[number_of_layers - 1].copyMatrix();
            return output;
        }

        float loss(Matrix output, Matrix expected_output){
            check_if_vector_is_valid(output, number_of_layers - 1);
            check_if_vector_is_valid(expected_output, number_of_layers - 1);
            float loss = 0;
            for (int node_index = 0; node_index < shape[number_of_layers - 1]; node_index++){
                loss += (output.matrix[node_index][0] - expected_output.matrix[node_index][0]) * (output.matrix[node_index][0] - expected_output.matrix[node_index][0]);
            }
            return loss;
        }

        void backward_(Matrix output, Matrix expected_output){
            check_if_vector_is_valid(output, number_of_layers - 1);
            check_if_vector_is_valid(expected_output, number_of_layers - 1);
            /*
            First We Treat The Function As A Function With The Nodes As The Variables
            Calculate Gradient For Last Layer
            */
            layers_gradient[number_of_layers - 1].freeMatrix_();
            layers_gradient[number_of_layers - 1] = output - expected_output;
            biases_gradient[number_of_layers - 1].freeMatrix_();
            biases_gradient[number_of_layers - 1] = output - expected_output;
            /*
            Now We Calculate The Gradient The Following Way:
            -dLoss/dCurrentNode = SUM(dLoss/dUpperNode * dUpperNode/dCurrentNode)
            -If we use logistic regression then dLoss/dCurrentNode = SUM(dLoss/dUpperNode * dUpperNode/dSigmoid * dSigmod/dCurrentNode)
            -The Reason We Use The Sum Is The Chain Rule
            -And dUpperNode/dCurrentNode = Weight Between Nodes = Weights[current_layer_index][upper_node_index][current_node_index]
            */
            for (int layer_index = number_of_layers - 2; layer_index >= 0; layer_index--){
                for (int current_node_index = 0; current_node_index < shape[layer_index]; current_node_index++){
                    layers_gradient[layer_index].matrix[current_node_index][0] = 0;
                    for (int upper_node_index = 0; upper_node_index < shape[layer_index + 1]; upper_node_index++){
                        float contribute_from_upper_node =
                            weights[layer_index].matrix[upper_node_index][current_node_index] *
                            layers_gradient[layer_index + 1].matrix[upper_node_index][0];
                        if (logistic_regression){
                            contribute_from_upper_node *= sigmoid_d(layers_gradient[layer_index + 1].matrix[upper_node_index][0]);
                        }
                        layers_gradient[layer_index].matrix[current_node_index][0] += contribute_from_upper_node;

                    }
                }
            }

            /*
            Now We Calculate The Gradient For The Weights The Following Way:
                dLoss/dWeight = dLoss/dUpperNode * dUpperNode / Weight
                We Calculated dLoss/dUpperNode In The Above Section
                dUpperNode / Weight = The Current Value In The Node Below
            */
            for (int layer_index = number_of_layers - 2; layer_index >= 0; layer_index--){
                for (int current_node_index = 0; current_node_index < shape[layer_index]; current_node_index++){
                    for (int upper_node_index = 0; upper_node_index < shape[layer_index + 1]; upper_node_index++){
                        float current_weight_gradient =
                            layers_gradient[layer_index + 1].matrix[upper_node_index][0] *
                            layers[layer_index].matrix[current_node_index][0];
                        weights_gradient[layer_index].matrix[upper_node_index][current_node_index] = current_weight_gradient;
                    }
                }
            }
            /*
            Now We Calculate The Gradient For The Biases The Following Way:
                dLoss/dBias = dLoss/dUpperNode * dUpperNode / dBias
                We Calculated dLoss/dUpperNode In The Above Section
                dUpperNode / Bias = 1
            */
            for (int layer_index = number_of_layers - 1; layer_index >= 0; layer_index--){
                biases_gradient[layer_index].freeMatrix_();
                biases_gradient[layer_index] = layers_gradient[layer_index].copyMatrix();
            }

        }

        void step_(float learning_rate){
            //Update Weights
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                weights[layer_index] -= weights_gradient[layer_index] * learning_rate;
            }

            //Update Biases
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                biases[layer_index] -= biases_gradient[layer_index] * learning_rate;
            }

        }

        void step_back_(float learning_rate){
            //Update Weights Back
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                weights[layer_index] += weights_gradient[layer_index] * learning_rate;
            }

            //Update Biases
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                biases[layer_index] += biases_gradient[layer_index] * learning_rate;
            }

        }

        Matrix train_return_output_(Matrix input, Matrix expected_output){
            Matrix output(shape[number_of_layers - 1], 1);
            output = forward(input);
            float l = loss(output, expected_output);
            backward_(output, expected_output);
            step_(learning_rate);
            return output;
        }

        void train_(Matrix input, Matrix expected_output, float *l = NULL){
            float current_learning_rate = learning_rate;
            Matrix output(shape[number_of_layers - 1], 1);
            output = forward(input);
            float current_loss = loss(output, expected_output);
            if (l != NULL){
                *l = current_loss;
            }
            backward_(output, expected_output);
            step_(current_learning_rate);
            output.freeMatrix_();
        }

        void printNetwork(){
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                printf("Layer %d: ", layer_index);
                for (int row_index = 0; row_index < layers[layer_index].rows; row_index++){
                    printf("%.2f, ", layers[layer_index].matrix[row_index][0]);
                }
                printf("\n");
            }
            printf("\n");
        }

        void printBiases(){
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                printf("Layer %d: ", layer_index);
                for (int row_index = 0; row_index < layers[layer_index].rows; row_index++){
                    printf("%.2f, ", biases[layer_index].matrix[row_index][0]);
                }
                printf("\n");
            }
            printf("\n");
        }

        void printWeights(){
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                printf("From Layer %d To Layer %d:\n", layer_index, layer_index + 1);
                weights[layer_index].printMatrix();
            }
            printf("\n");
        }

        void freeAllMatrices_(){
            //Free All Layers
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                layers[layer_index].freeMatrix_();
                layers_gradient[layer_index].freeMatrix_();
            }
            //Free All Weights
            for (int layer_index = 0; layer_index < number_of_layers - 1; layer_index++){
                weights[layer_index].freeMatrix_();
                weights_gradient[layer_index].freeMatrix_();
            }
            //Free All Biases
            for (int layer_index = 0; layer_index < number_of_layers; layer_index++){
                biases[layer_index].freeMatrix_();
                biases_gradient[layer_index].freeMatrix_();
            }
        }

        void check_if_vector_is_valid(Matrix V, int layer_index){
            //Accepted Vectors Are Either 1xshape[0] or shape[0]x1. We Want V To Be Of Shape[0]x1
            if (V.rows != shape[layer_index] && V.columns != shape[layer_index]){
                printf("Shape[%d] = %d | V shape: %d X %d\n", layer_index, shape[layer_index], V.rows, V.columns);
                throw std::invalid_argument("Shape Of Vector Must Be Equal To Network Shape\n");
            }
            if (V.columns != 1){
                throw std::invalid_argument("Output Must Be A Vector In The Form Of Rows X 1\n");
            }
        }

};

float sigmoid(float x){
    return pow(M_E, x) / (1 + pow(M_E, x));
}

float sigmoid_d(float x){
    return sigmoid(x) * sigmoid(1 - x);
}
