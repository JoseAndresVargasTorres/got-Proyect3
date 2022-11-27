#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <client.hpp>

using namespace std;

vector< pair <std::string, std::string> > vect;
string cadena_descomprimida;
string str = "";
string str2 = "";

// A Tree node
struct Node
{
	char ch;
	int freq;
	Node *left, *right;
};

// Function to allocate a new tree node
Node* getNode(char ch, int freq, Node* left, Node* right)
{
	Node* node = new Node();

	node->ch = ch;
	node->freq = freq;
	node->left = left;
	node->right = right;

	return node;
}

// Comparison object to be used to order the heap
struct comp
{
	bool operator()(Node* l, Node* r)
	{
		// highest priority item has lowest frequency
		return l->freq > r->freq;
	}
};

// traverse the Huffman Tree and store Huffman Codes
// in a map.
void encode(Node* root, string str,
			unordered_map<char, string> &huffmanCode)
{
	if (root == nullptr)
		return;

	// found a leaf node
	if (!root->left && !root->right) {
		huffmanCode[root->ch] = str;
	}

	encode(root->left, str + "0", huffmanCode);
	encode(root->right, str + "1", huffmanCode);
}

// traverse the Huffman Tree and decode the encoded string
void decode(Node* root, int &index, string str)
{
	if (root == nullptr) {
		return;
	}

	// found a leaf node
	if (!root->left && !root->right)
	{
		cout << root->ch;
		return;
	}

	index++;

	if (str[index] =='0')
		decode(root->left, index, str);
	else
		decode(root->right, index, str);
}

// Builds Huffman Tree and decode given input text
void buildHuffmanTree(string text)
{

	vect.clear();
    str = "";
    str2 = "";
    cadena_descomprimida = "";
	
	// count frequency of appearance of each character
	// and store it in a map
	unordered_map<char, int> freq;
	for (char ch: text) {
		freq[ch]++;
	}

	// Create a priority queue to store live nodes of
	// Huffman tree;
	priority_queue<Node*, vector<Node*>, comp> pq;

	// Create a leaf node for each character and add it
	// to the priority queue.
	for (auto pair: freq) {
		pq.push(getNode(pair.first, pair.second, nullptr, nullptr));
	}

	// do till there is more than one node in the queue
	while (pq.size() != 1)
	{
		// Remove the two nodes of highest priority
		// (lowest frequency) from the queue
		Node *left = pq.top(); pq.pop();
		Node *right = pq.top();	pq.pop();

		// Create a new internal node with these two nodes
		// as children and with frequency equal to the sum
		// of the two nodes' frequencies. Add the new node
		// to the priority queue.
		int sum = left->freq + right->freq;
		pq.push(getNode('\0', sum, left, right));
	}

	// root stores pointer to root of Huffman Tree
	Node* root = pq.top();

	// traverse the Huffman Tree and store Huffman Codes
	// in a map. Also prints them
	unordered_map<char, string> huffmanCode;
	encode(root, "", huffmanCode);

	//cout << "Huffman Codes are :\n" << '\n';
	for (auto pair: huffmanCode) {
		str2 += pair.first;
		str2 += pair.second;
		str2 += "/";
		//cout << pair.first << " " << pair.second << '\n';
	}

	//cout << "\nOriginal string was :\n" << text << '\n';

	// print encoded string
	str = "";
	for (char ch: text) {
		str += huffmanCode[ch];
	}

	//cout << "\nEncoded string is :\n" << str << '\n';

	// traverse the Huffman Tree again and this time
	// decode the encoded string
	/*int index = -1;
	cout << "\nDecoded string is: \n";
	while (index < (int)str.size() - 2) {
		decode(root, index, str);
	}*/
}

string descomprimir_data(string codigo_binario, string simbolo_codigo){
	
	vect.clear();
    str = "";
    str2 = "";
    cadena_descomprimida = "";
	
	// Reconstruir vector desde el string simbolo_codigo
	string simbolo;
	string codigo;
	int i;
	while(simbolo_codigo!=""){
		codigo = "";
		simbolo = simbolo_codigo[0];
		simbolo_codigo.erase(0, 1);
		i = 0;
		while (simbolo_codigo[i] != '/')
    	{
			codigo += simbolo_codigo[i];
			i++;
		}
		simbolo_codigo.erase(0, i+1);	
		vect.push_back( make_pair(simbolo, codigo));
	}

	// Descomprimir data
	string temporal = "";
    cadena_descomprimida = "";
    while(codigo_binario!=""){
        temporal += codigo_binario[0];
        for(unsigned int i=0; i<vect.size(); i++){
            if(temporal==vect[i].second){
                cadena_descomprimida += vect[i].first;
                temporal = "";
                break;
            }
        }
        codigo_binario.erase(0, 1);
    }

	return cadena_descomprimida;
}

string pedir_codigoBinario(){
	return str;
}

string pedir_simboloCodigo(){
	return str2;
}

string generar_string_de_archivo(string ruta){
    fstream ficheroEntrada;
    string nombre = ruta;
    string linea_texto;
    string texto_final;

    ficheroEntrada.open(nombre.c_str(), ios::in);
    if (ficheroEntrada.is_open())
    {
        while (!ficheroEntrada.eof())
        {
            getline(ficheroEntrada, linea_texto);
            texto_final += linea_texto + "\n";
        }
        ficheroEntrada.close();
    }
    else
    {
        spdlog::error("Fichero inexistente o faltan permisos para abrirlo (Huffman)");
    }
	texto_final.erase(texto_final.size() - 1);
	return texto_final;
}