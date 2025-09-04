#include <iostream>
#include <random>
#include <vector>
#include <utility>
#include <cmath>
#include <fstream>

double GenDouble(double a, double b) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dis(a, b);
	return dis(gen);
}

struct Dot
{
	double x;
	double y;
};

std::vector<Dot> GenDots(int N)
{
	std::vector<Dot> Dots(N);
	for (int i = 0; i < N; i++)
	{
		Dots[i].x = GenDouble(-20, 20);
		Dots[i].y = GenDouble(-20, 20);
	}
	return Dots;
}

std::vector<std::vector<double>> FormationOfMatrix(std::vector<Dot>& Dots)
{
	std::vector<std::vector<double>> Matrix(Dots.size(), std::vector<double>(Dots.size()));
	for (int i = 0; i < Dots.size() - 1; i++)
	{
		for (int j = i + 1; j < Dots.size(); j++)
		{
			Matrix[i][j] = Matrix[j][i] = sqrt(pow((Dots[i].x - Dots[j].x), 2) + pow((Dots[i].y - Dots[j].y), 2));
		}
	}
	return Matrix;
}

std::vector<std::vector<int>> AlgorithmPrima(std::vector<std::vector<double>>& Matrix, int& n)
{
	std::vector<std::vector<int>> Tree;

	double wmin;
	int i, j, vm, * B = new int[n];
	B[0] = -1;
	for (i = 1; i < n; i++) B[i] = 0;
	for (i = 1; i < n; i++) 
	{
		wmin = INFINITY; vm = 0;
		for (j = 1; j < n; j++)
			if (B[j] != -1 && wmin > Matrix[j][B[j]])
			{
				vm = j; wmin = Matrix[j][B[j]];
			}
		if (!vm) return Tree;
		std::vector<int> edge = { vm, B[vm] };
		Tree.push_back(edge); B[vm] = -1;
		for (j = 1; j < n; j++)
			if (B[j] != -1 && Matrix[j][B[j]] > Matrix[j][vm])
				B[j] = vm;
	} 
	delete[]B;

	return Tree;
}

void to_file(std::vector<Dot>& Dots, std::vector<std::vector<int>>& Tree, int* Ind, int N_K)
{
	//write_dots
	std::ofstream dots_file("dots.txt");
	if (dots_file.is_open()) 
	{
		for (int i = 0; i < Dots.size(); i++)
		{
			dots_file << Dots[i].x << " " << Dots[i].y << "\n";
		}
		dots_file.close();
	}
	else {
		std::cerr << "error!" << std::endl;
	}

	//write tree_edges
	std::ofstream tree_file("tree.txt");
	if (tree_file.is_open()) 
	{
		for (int i = 0; i < Dots.size() - 1; i++)
		{
			tree_file << Tree[i][0] << " " << Tree[i][1] << "\n";
		}
		tree_file.close();
	}
	else 
	{
		std::cerr << "error!" << std::endl;
	}

	//write all clusters 
	std::ofstream clusters_file("clusters.txt");
	if (clusters_file.is_open()) 
	{
		for (int i = 0; i < N_K; i++) 
		{
			clusters_file << Tree[Ind[i]][0] << " " << Tree[Ind[i]][1] << "\n";
		}
		clusters_file.close();
	}
	else 
	{
		std::cerr << "error!" << std::endl;
	}
}

void swap(int* a, int* b)
{
	int c;
	c = *a;
	*a = *b;
	*b = c;
}

void SortShell(double* A, int* Ind, int n)
{
	int i, j, h;
	for (h = 1; h <= n / 9; h = h * 3 + 1);
	while (h >= 1)
	{
		for (i = h; i < n; i++)
			for (j = i - h; (j >= 0) && (A[Ind[j]] > A[Ind[j + h]]); j -= h)
				swap(&Ind[j], &Ind[j + h]);
		h = (h - 1) / 3;
	}
}

void cdeep(std::vector<std::vector<double>>& Matrix, int n, int CurrVer, int* R, int CurrNum)
{
	R[CurrVer] = CurrNum;
	for (int i = 0; i < n; i++)
		if (Matrix[CurrVer][i] && !R[i])
			cdeep(Matrix, n, i, R, CurrNum);
}

int* get_comp(std::vector<std::vector<double>>& Matrix, int n, int& CurrNum)
{
	int i, * R = new int[n];
	for (i = 0; i < n; i++) R[i] = 0;
	for (CurrNum = i = 0; i < n; i++)
		if (!R[i])
		{
			CurrNum++;
			cdeep(Matrix, n, i, R, CurrNum);
		}
	return R;
}

struct Borders
{
	double min_x;
	double min_y;
	double max_x;
	double max_y;
};

void allminpath(std::vector<std::vector<double>>& Matrix, int n)
{
	int i, j, l;
	for (l = 0; l < n; l++)
	{
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < n; j++)
			{
				Matrix[i][j] = std::min(Matrix[i][j], Matrix[i][l] + Matrix[l][j]);
			}
		}
	}
}

struct Centroid
{
	int ver;
	double minDistance;
};

Centroid FindCentroid(std::vector<std::vector<double>>& MatrixMinPaths, int* NEW_R, int comp)
{
	Centroid centroid; double sum;
	centroid.ver = -1; centroid.minDistance = INFINITY;
	for (int i = 0; i < MatrixMinPaths[0].size(); i++)
	{
		sum = 0;
		if (NEW_R[i] == comp)
		{
			for (int j = 0; j < MatrixMinPaths[0].size(); j++)
			{
				if ((NEW_R[j] == comp) && (i!=j) && (MatrixMinPaths[i][j] < INFINITY))
				{
					sum += MatrixMinPaths[i][j];
				}
			}
		}
		if ((sum < centroid.minDistance) && (sum != 0))
		{
			centroid.minDistance = sum;
			centroid.ver = i;
		}
	}
	return centroid;
}

void PrintMatrix(std::vector<std::vector<double>>& Matrix)
{
	for (int i = 0; i < Matrix[0].size(); i++)
	{
		for (int j = 0; j < Matrix[0].size(); j++)
		{
			printf("%7.2f", Matrix[i][j]);
		}
		printf("\n");
	}
	printf("------------------------------------------------------------------\n");
}

void PrintDots(std::vector<Dot>& Dots)
{
	for (int i = 0; i < Dots.size(); i++)
	{
		printf("(%.2f;%.2f)\n", Dots[i].x, Dots[i].y);
	}
	printf("------------------------------------------------------------------\n");
}

void PrintTree(std::vector<std::vector<int>>& Tree)
{
	for (int i = 0; i < Tree.size(); i++)
	{
		printf("%d %d\n", Tree[i][0], Tree[i][1]);
	}
	printf("------------------------------------------------------------------\n");
}

void PrintSortedTree(std::vector<std::vector<int>>& Tree, int* Ind, double* weigths)
{
	for (int i = 0; i < Tree.size(); i++)
	{
		printf("%d %d; %.2f\n", Tree[Ind[i]][0], Tree[Ind[i]][1], weigths[Ind[i]]);
	}
	printf("------------------------------------------------------------------\n");
}

void PrintVector(int* R, int n)
{
	for (int i = 0; i < n; i++)
	{
		std::cout << R[i] << " ";
	}
	printf("\n------------------------------------------------------------------\n");
}

void main()
{
	//start initialization
	int N;
	std::cout << "enter N: ";
	std::cin >> N;
	printf("------------------------------------------------------------------\n");
	std::vector<Dot> Dots = GenDots(N);
	std::cout << "Dots:\n";
	PrintDots(Dots);

	//start matrix
	std::cout << "START_Matrix:\n";
	std::vector<std::vector<double>> Matrix(Dots.size(), std::vector<double>(Dots.size()));
	Matrix = FormationOfMatrix(Dots);
	PrintMatrix(Matrix);

	//satrt components check
	int* R = new int[N]; int CurrNum = 0;
	get_comp(Matrix, N, CurrNum);
	printf("Components = %d\n", CurrNum);

	//min tree
	std::cout << "Tree:\n";
	std::vector<std::vector<int>> Tree;
	Tree = AlgorithmPrima(Matrix, N);
	PrintTree(Tree);

	//sort edges by weigths
	double* weigths = new double[Tree.size()];
	int* Ind = new int[Tree.size()];
	for (int i = 0; i < Tree.size(); i++)
	{
		weigths[i] = Matrix[Tree[i][0]][Tree[i][1]];
		Ind[i] = i;
	}
	std::cout << "Sorted edges by weigths:\n";
	SortShell(weigths, Ind, Tree.size());
	PrintSortedTree(Tree, Ind, weigths);

	//initialization K - num of clusters + NEW_matrix
	int K;
	std::cout << "enter K: ";
	std::cin >> K;
	int N_K = N - K;
	std::vector<std::vector<double>> NEW_Matrix(Dots.size(), std::vector<double>(Dots.size()));
	for (int i = 0; i < N_K; i++)
	{
		NEW_Matrix[Tree[Ind[i]][0]][Tree[Ind[i]][1]] = NEW_Matrix[Tree[Ind[i]][1]][Tree[Ind[i]][0]] = weigths[Ind[i]];
	}
	std::cout << "NEW_Matrix:\n";
	PrintMatrix(NEW_Matrix);

	//for visualization
	to_file(Dots, Tree, Ind, N_K);

	//num of clusters
	int* NEW_R = new int[Dots.size()]; int NEW_CurrNum = 0;
	NEW_R = get_comp(NEW_Matrix, Dots.size(), NEW_CurrNum);
	printf("Components (Clusters) = %d\n", NEW_CurrNum);
	PrintVector(NEW_R, Dots.size());

	//for each cluster - num of dots
	int k = 1, num_dots = 0;
	while (k <= NEW_CurrNum)
	{
		std::cout << "Cluster #" << k << ": ";
		for (int i = 0; i < Dots.size(); i++)
		{
			if (NEW_R[i] == k)
			{
				num_dots++;
				std::cout << i << " ";
			}
		}
		std::cout << "(num_dots = " << num_dots << ")\n";
		printf("------------------------------------------------------------------\n");
		num_dots = 0;
		k++;
	}

	//for each cluster - min_x, min_y, max_x, max_y
	std::vector<Borders> componentBorders(NEW_CurrNum + 1);
	for (int i = 1; i <= NEW_CurrNum; ++i) {
		componentBorders[i].min_x = INFINITY;
		componentBorders[i].min_y = INFINITY;
		componentBorders[i].max_x = -(INFINITY);
		componentBorders[i].max_y = -INFINITY;
	}
	for (int i = 0; i < Dots.size(); ++i) {
		int comp = NEW_R[i];
		componentBorders[comp].min_x = std::min(componentBorders[comp].min_x, Dots[i].x);
		componentBorders[comp].min_y = std::min(componentBorders[comp].min_y, Dots[i].y);
		componentBorders[comp].max_x = std::max(componentBorders[comp].max_x, Dots[i].x);
		componentBorders[comp].max_y = std::max(componentBorders[comp].max_y, Dots[i].y);
	}
	for (int i = 1; i <= NEW_CurrNum; ++i) {
		std::cout << "Cluster #" << i << ":\n";
		std::cout << "min_x: " << componentBorders[i].min_x << ", min_y: " << componentBorders[i].min_y << "\n";
		std::cout << "max_x: " << componentBorders[i].max_x << ", max_y: " << componentBorders[i].max_y << "\n";
		printf("------------------------------------------------------------------\n");
	}

	//matrix for AllMinPath
	for (int i = 0; i < Dots.size(); i++)
	{
		for (int j = 0; j < Dots.size(); j++)
		{
			if (!NEW_Matrix[i][j])
			{
				NEW_Matrix[i][j] = INFINITY;
			}
		}
	}

	//AllMinPath
	allminpath(NEW_Matrix, N);
	std::cout << "Matrix_All_Min_Paths:\n";
	for (int i = 0; i < Dots.size(); i++)
	{
		NEW_Matrix[i][i] = 0;
	}
	PrintMatrix(NEW_Matrix);
	
	//centroids for each cluster
	int comp = 1;
	Centroid centroid; 
	int kol;
	while (comp <= NEW_CurrNum)
	{
		centroid = FindCentroid(NEW_Matrix, NEW_R, comp);

		//
		if (centroid.ver == -1)
		{
			kol = 0;
			for (int i = 0; i < Dots.size(); i++)
			{
				if (NEW_R[i] == comp)
				{
					kol++;
					centroid.ver = i;
					centroid.minDistance = 0;
				}
			}
			if (kol > 1)
			{
				centroid.ver = -1;
				centroid.minDistance = INFINITY;
			}
		}
		//

		printf("Centroid for cluster #%d:\nver = %d;\nminDistance = %.2f;\n", comp, centroid.ver, centroid.minDistance);
		printf("------------------------------------------------------------------\n");
		comp++;
	}
}