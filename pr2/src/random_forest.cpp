#include "random_forest.h"

using namespace std;

clock_t tstart;

float training_ratio;
float bagging_ratio;
float pure_standard;
int forest_size;
double correct_rate;

vector<Data> d; // the whole data set
vector<Data> training_sset;
vector<Data> validation_sset;
vector<Data> OOB_sset;

vector<Tree> forest;


Node::Node() {
	attribute = 0;
	threshold = 0;
	left = NULL;
	right = NULL;
	parent = NULL;
	isleaf = 0;
	memset(label, 0, sizeof(label));
}

// build during creating a tree object
Tree::Tree() {
	root = new Node;
	bagging();
	buildTree(root);
}

// select random data into root dataset
// push remain data into out-of-bag subset
// select 2(root of 4) random attribute to be used(can't be used later)
void Tree::bagging() {
	// data bagging
	int bagsize = training_sset.size() * bagging_ratio;
	random_shuffle(training_sset.begin(), training_sset.end(), genRandom);
	int i=0;
	while(i < bagsize) {
		root->dataset.push_back(training_sset[i]);
		i++;
	}
	while(i < training_sset.size()) {
		OOB_sset.push_back(training_sset[i]);
		i++;
	}
	// attribute bagging
	used_attr.push_back(0);
	used_attr.push_back(0);
	used_attr.push_back(1);
	used_attr.push_back(1);
	random_shuffle(used_attr.begin(), used_attr.end(), genRandom);

	return;
}

// calculate gini index of a set of data
double Tree::giniIndex(vector<Data> v) {
	if(v.size() == 0) return 0;

	int cnt[3];
	double index = 1;
	memset(cnt, 0, sizeof(cnt));
	for(int i=0; i<v.size(); i++) {
		if(strcmp(v[i].label, "Iris-setosa") == 0) cnt[0]++;
		else if(strcmp(v[i].label, "Iris-virginica") == 0) cnt[1]++;
		else cnt[2]++;
	}
	for(int i=0; i<3; i++) {
		double pk = (double)cnt[i] / v.size();
		index -= pk * pk;
	}
	// printf("%d, %d, %d, %lf\n", cnt[0], cnt[1], cnt[2], index);
	return index;
}

// calculate the total impurity of a node dataset
double Tree::impurity(Node *n) {
	double g1 = giniIndex(n->left->dataset);
	double g2 = giniIndex(n->right->dataset);
	double n1 = (double)n->left->dataset.size() / n->dataset.size();
	double n2 = (double)n->right->dataset.size() / n->dataset.size();

	// printf("\t\t(%f*%f + %f*%f)\n", n1, g1, n2, g2);

	return (n1*g1 + n2*g2);
}

// only split data set into 2 children
// you have to delete children explicitly if unused
void Tree::split(Node *n, int attr_num, double threshold) {
	n->left = new Node;
	n->right = new Node;
	n->left->parent = n;
	n->right->parent = n;

	for(int i=0; i<n->dataset.size(); i++) {
		float val = n->dataset[i].attr[attr_num];
		if(val <= threshold) {
			n->left->dataset.push_back(n->dataset[i]);
		}
		else {
			n->right->dataset.push_back(n->dataset[i]);
		}
	}
	// for(int i=0; i<n->left->dataset.size(); i++) {
	// 	printf("%f, %f, %f, %f, %s\n", n->left->dataset[i].attr[0], n->left->dataset[i].attr[1], n->left->dataset[i].attr[2], n->left->dataset[i].attr[3], n->left->dataset[i].label);
	// }
	return;
}

// return the best threshold value of an attribute
double Tree::selectThreshold(Node *n, int attr_num) {
	vector<float> val;
	double min_impurity = 1;
	double best_threshold;

	for(int i=0; i<n->dataset.size(); i++) {
		val.push_back(n->dataset[i].attr[attr_num]);
	}
	sort(val.begin(), val.end());
	for(int i=1; i<val.size(); i++) {
		split(n, attr_num, (val[i]+val[i-1])/2);
		double n_impurity = impurity(n);
		// printf("\t\t-> impurity: %f\n", n_impurity);
		if(min_impurity > n_impurity) {
			min_impurity = n_impurity;
			best_threshold = (val[i]+val[i-1])/2;
		}
		delete n->left;
		delete n->right;
	}
	// printf("\tthreshold: %f\n", best_threshold);

	return best_threshold;
}

// select a best attribute with threshold and split node
void Tree::selectAttribute(Node *n) {
	int best_attribute;
	double best_threshold;
	double min_impurity = 1;

	for(int i=0; i<4; i++) {
		if(used_attr[i]) continue;
		double thold = selectThreshold(n, i);
		split(n, i, thold);
		double n_impurity = impurity(n);
		if(min_impurity > n_impurity) {
			min_impurity = n_impurity;
			best_threshold = thold;
			best_attribute = i;
		}
		delete n->left;
		delete n->right;
	}

	split(n, best_attribute, best_threshold);
	n->attribute = best_attribute;
	n->threshold = best_threshold;
	used_attr[best_attribute] = 1;
	// printf("attribute: %d\n", best_attribute);
	return;
}

// based on the gini index of a dataset to check if it is pure
// determined by the pure_standard(default: 0.0)
int Tree::isPure(vector<Data> v) {
	return (giniIndex(v) > pure_standard) ? 0 : 1;
}

int Tree::remainAttr() {
	int cnt = 0;
	for(int i=0; i<used_attr.size(); i++) {
		if(used_attr[i] == 0) {
			cnt++;
		}
	}
	return cnt;
}

// read through the dataset and set the most label
void Tree::selectLabel(Node* n) {
	int cnt[3];
	memset(cnt, 0, sizeof(cnt));
	for(int i=0; i<n->dataset.size(); i++) {
		if(strcmp(n->dataset[i].label, "Iris-setosa") == 0) {
			cnt[0]++;
		}
		else if (strcmp(n->dataset[i].label, "Iris-virginica") == 0) {
			cnt[1]++;
		}
		else cnt[2]++;
	}
	int maj = max(cnt[0], max(cnt[1], cnt[2]));
	if(maj == cnt[0]) {
		strcpy(n->label, "Iris-setosa");
	}
	else if(maj == cnt[1]) {
		strcpy(n->label, "Iris-virginica");
	}
	else {
		strcpy(n->label, "Iris-versicolor");
	}
	return;
}

// check the necessity to split the node(data purity, remain unused attributes)
// if not necessary, set the node to leaf and set label
int Tree::checkLeaf(Node *n) {
	if(n->dataset.size() != 0 && !isPure(n->dataset) && remainAttr() != 0) return 0;
	n->isleaf = 1;
	selectLabel(n);
	return 1;
}

// recursively split the nodes
void Tree::buildTree(Node *n) {
	if(checkLeaf(n)) return;
	selectAttribute(n);
	buildTree(n->left);
	buildTree(n->right);
}

// for random_shuffle()
int genRandom(int num) { return rand()%num; }

// set timer for calculating excecution time
void timeStart() {
	tstart = clock();
}

void readData(const char* fpath) {
	FILE* fp;
	if((fp = fopen(fpath, "r")) == NULL) {
		perror("file not exists");
		exit(-1);
	}

	char buf[BUFSIZE];

	while(fgets(buf, BUFSIZE, fp) != NULL) {
		float tmp[4];
		Data dtmp;
		sscanf(buf, "%f,%f,%f,%f,%s", &tmp[0], &tmp[1], &tmp[2], &tmp[3], dtmp.label);
		dtmp.attr.push_back(tmp[0]);
		dtmp.attr.push_back(tmp[1]);
		dtmp.attr.push_back(tmp[2]);
		dtmp.attr.push_back(tmp[3]);
		d.push_back(dtmp);
	}

	fclose(fp);

	// for(int i=0; i<d.size(); i++) {
	// 	printf("%.1f, %.1f, %.1f, %.1f, %s\n", d[i].attr[0], d[i].attr[1], d[i].attr[2], d[i].attr[3], d[i].label);
	// }
}

// divide the original dataset into training subset and validation subset
void divideDataset() {
	int train_num = d.size() * training_ratio;
	int validate_num = d.size() - train_num;
	random_shuffle(d.begin(), d.end(), genRandom);
	int i = 0;
	while(i < train_num) {
		training_sset.push_back(d[i]);
		i++;
	}
	while(i < d.size()) {
		validation_sset.push_back(d[i]);
		i++;
	}
}

void buildForest() {
	for(int i=0; i<forest_size; i++) {
		Tree t;
		forest.push_back(t);
	}
}

// traverse through the tree and return the classify result
char* traverse(Node *n, Data data) {
	if(n->isleaf) {
		return n->label;
	}
	// else select a way to go
	if((data.attr[n->attribute] <= n->threshold) && n->left) {
		return traverse(n->left, data);
	}
	else if(n->right) {
		return traverse(n->right, data);
	}
	fprintf(stderr, "traverse error\n");
	exit(-1);
}

// return the majority vote of the forest
char* ensemble(Data data) {
	int cnt[3];
	char result[LABELSIZE]; // result for each tree
	char* ret;
	memset(cnt, 0, sizeof(cnt));
	for(int i=0; i<forest.size(); i++) {
		strcpy(result, traverse(forest[i].root, data));
		if(strcmp(result, "Iris-setosa") == 0) {
			cnt[0]++;
		}
		else if(strcmp(result, "Iris-virginica") == 0) {
			cnt[1]++;
		}
		else cnt[2]++;
	}
	int maj = max(cnt[0], max(cnt[1], cnt[2]));
	if(maj == cnt[0]) {
		ret = strdup("Iris-setosa");
	}
	else if(maj == cnt[1]) {
		ret = strdup("Iris-virginica");
	}
	else {
		ret = strdup("Iris-versicolor");
	}

	return ret;
}

// validation and return the correct rate
double correctRate(vector<Data> sset) {
	int data_n = sset.size();
	int cnt_correct = 0;
	for(int i=0; i<data_n; i++) {
		if(strcmp(sset[i].label, ensemble(sset[i])) == 0) {
			cnt_correct++;
		}
	}

	return ((double)cnt_correct / data_n);
}

void printResult() {
	printf("%-4.2f/%-8.2f", training_ratio, 1-training_ratio); // traning/validation data
	printf("%-7.2f", bagging_ratio); // bagging ratio
	printf("%-7.2f", pure_standard); // gini pure standard
	printf("%-6d", forest_size); // forest size
	printf("%-10f", correctRate(OOB_sset)); // oob correct rate
	printf("%-10f", correctRate(validation_sset)); // validation correct rate
	printf("%-5f", (double)(clock()-tstart) / CLOCKS_PER_SEC); // execution times
	printf("\n");
}

// argv[1] = data file path
// argv[2] = training_ratio
// argv[3] = bagging_ratio
// argv[4] = pure_standard
// argv[5] = forest_size
int main(int argc, char *argv[]) { 
	training_ratio = atof(argv[2]);
	bagging_ratio = atof(argv[3]);
	pure_standard = atof(argv[4]);
	forest_size = atof(argv[5]);

	srand(time(NULL));
	readData(argv[1]);
	divideDataset();
	buildForest();
	// Tree t; t.printDataset();
	printResult();
	// t.printDataset();

	
	return 0;
}




// ******************for debugging****************** //
int level = 0;

void Tree::printDataset() {
	printDataset(root);

}

void Tree::printDataset(Node* n) {
	printf("(%d, %f)\n", n->attribute, n->threshold);
	printDataset(n->dataset);

	level++;
	if(n->left != NULL) {
		printDataset(n->left);
	}
	if(n->right != NULL) {
		printDataset(n->right);
	}
	level--;
	return;
}

void Tree::printDataset(vector<Data> v) {
	for(int i=0; i<v.size(); i++) {
		printf("%f, %f, %f, %f, %s\n", v[i].attr[0], v[i].attr[1], v[i].attr[2], v[i].attr[3], v[i].label);
	}
	printf("-%d\n", level);
}