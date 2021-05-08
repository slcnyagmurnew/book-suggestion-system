#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// benzerlik oran�n� hesaplayabilmek i�in sim fonksiyonunda kullan�lan mean
double mean(double data1[], double data2[], int n) 
{ 
    double sum = 0.0;
	int i, num = 0; 
    for(i = 0; i < n; i++){
    	if(data1[i] == 0 || data2[i] == 0){
		}
    	else{
    		sum = sum + data1[i];
    		num++;
		}
	}
    return sum / num;
}
// tahmini puan �retebilmek i�in pred fonksiyonunda kullan�lan mean
double mean_for_estimate(double data[], int n)
{
	double sum = 0.0;
	int i, num = 0;
	for(i = 0; i < n; i++){
		if(data[i] == 0){
		}
		else{
			sum = sum + data[i];
			num++;
		}
	}
	return sum / num;
}
// similarityi hesaplayan fonksiyon, sim form�l�n�n ger�eklenmesi
double calc(double data1[], double data2[], int n) 
{ 
    double sum = 0.0, normalize_1 = 0.0, normalize_2 = 0.0, similar = 0.0;
	int i; 
	double mean1 = mean(data1, data2, n);
	double mean2 = mean(data2, data1, n);
    for(i = 0; i < n; i++){
    	if(data1[i] == 0 || data2[i] == 0){
		}
    	else{
    		sum = sum + ((data1[i] - mean1) * (data2[i] - mean2));
    		normalize_1 += pow(data1[i] - mean1, 2);
			normalize_2 += pow(data2[i] - mean2, 2);	
		}
	}
	similar = sum / (sqrt(normalize_1) * sqrt(normalize_2));
	return similar;
}
// new user benzerliklerinin aras�ndan k adet en benzeri bulmak icin kullan�lan fonksiyon
void find_max(double data[], int * index, double * similarity, int k, int U)
{
	int i;
    double max = data[0]; 
    *index = 0;
    for (i = 1; i < U; i++){
    	if (data[i] > max){
        	max = data[i];
        	*index = i; // en y�ksek index parametre �zerinden de�i�tirilir
		}
	}
	similarity[k] = max; // dizide parametre olarak g�nderilen k indis de�erine yerle�tirilir
}
// csv dosyas�n�n ayr��t�r�lmas� i�in kullan�lan string tokenizer benzeri fonksiyon
char * new_strtok(char * string, char const * delimiter)
{
	static char * source = NULL;
	char * p, * value = 0;
	if(string != NULL)
        source = string;
	if(source == NULL)    
		return NULL; // delimeter de�erine g�re Null durumu
	if((p = strpbrk(source, delimiter)) != NULL) {
	  *p  = 0;
	  value = source;
	  source = ++p;
	}
	return value;
}
// puan tahmini i�in kullan�lan fonksiyon
double estimate(int * index_of_users, int index_of_zero, int k,
				double * similarity, double * newuser, int n, double ** user_matrix)
{
	double sum = 0.0, num = 0.0;
	int i, j;
	double mean_of_newuser = mean_for_estimate(newuser, n); // ortalama
	for(i = 0; i < k; i++){
		double mean_of_user = mean_for_estimate(user_matrix[index_of_users[i]], n);
		sum += (similarity[i] * (user_matrix[index_of_users[i]][index_of_zero] - mean_of_user));
		num += similarity[i];
	}
	return mean_of_newuser + (sum / num);
}
// dosyadan bilgilerin okundu�u fonksiyon
void get_file(FILE * stream, double ** user_matrix, int n, int U, 
			  char books[20][20], char users[50][50])
{
	char line[1024];
    int num_of_user = 0, num_of_books = 0, i;
    fgets(line, 1024, stream);
	char * tmp = strdup(line);
    char * token = new_strtok(tmp, ";\n");
    for(i = 0; i < n; i++){
    	token = new_strtok(NULL,";\n");
    	strcpy(books[num_of_books], token); // kitaplar diziye al�n�r
    	num_of_books++;
	}
    free(tmp);
    while (num_of_user < U && fgets(line, 1024, stream))
    {
        char * tmp = strdup(line);
        char * token = new_strtok(tmp, ";\n");
        strcpy(users[num_of_user], token); // kullan�c�lar diziye al�n�r
        for(i = 0; i < n; i++){
        	token = new_strtok(NULL,";\n");
        	user_matrix[num_of_user][i] = atoi(token); // verilen puanlar matrise al�n�r
		}
        num_of_user++;
        free(tmp);
    }
}
// kullan�c� taraf�ndan girilen new user dosyadan fonksiyon yard�m�yla bulunur
void find_newuser(FILE * stream, char new_user[10], double * newuser_points, int n,
				 int * index_of_zeros, int * num_of_zeros)
{
	char line[1024];
	int i, matched = 0, zeros = 0;
    while (matched == 0 && fgets(line, 1024, stream))
    {
        char * tmp = strdup(line);
        char * token = new_strtok(tmp, ";\n");
        if(strcmp(new_user, token) == 0){
        	matched = 1; // kullan�c� bulundu
        	for(i = 0; i < n; i++){
	        	token = new_strtok(NULL,";\n");
	        	newuser_points[i] = atoi(token); // verilen puanlar diziye eklenir
	        	if(atoi(token) == 0){
	        		index_of_zeros[zeros] = i; // kullan�c�n�n puan vermedi�i kitaplar
    				zeros++;
				}
			}
	        free(tmp);
		}
    }
    *num_of_zeros = zeros; // puanlanmayanlar�n toplam say�s� parametre �zerinden de�i�tirilir
    fclose(stream);
}

int main()
{
	char data[1024], books[20][20], users[50][50];
	int cont = 1;
    do{
    	FILE* stream = fopen("RecomendationDataSet.csv", "r");
		if(stream == NULL){
			printf(" File can not be found..");
			exit(3);
		}
		int i, j, k, U, n, NU; // U: user sayisi, NU: new user sayisi, n: kitap sayisi, k: benzer ki�i sayisi
	    fgets(data, 1024, stream);
	    U = atoi(data); // user say�s� 
	    fgets(data, 1024, stream);
	    NU = atoi(data); // new user say�s�
	    fgets(data, 1024, stream);
	    n = atoi(data); // kitap say�s�
	    double ** user_matrix = (double **)malloc(U * sizeof(double *)); 
	    for (i = 0; i < U; i++) 
	        user_matrix[i] = (double *)malloc(n * sizeof(double));
    	char new_user[10];
		printf(" Enter the username for book recommendation : ");
		scanf("%s", new_user);
		printf(" Similar number of users : ");
		scanf("%d", &k); // benzerlik say�s�
		
		get_file(stream, user_matrix, n, U, books, users); // dosya verileri
	    
	    double * newuser_points = (double*)calloc(U, sizeof(double)); // yeni kullan�c�n�n puanlari
	    double * newuser_similarity = (double*)calloc(U, sizeof(double)); // yeni kullan�c�n�n benzerlikleri
	    double * similarity = (double*)calloc(U, sizeof(double)); // en yuksek benzerlikler
	    int * index_of_users = (int*) malloc(k * sizeof(int)); // en benzer kullan�c� indisleri
	    int * index = (int*)malloc(sizeof(int)); // en yuksek benzerlik indisi
	    int * num_of_zeros = (int*)malloc(sizeof(int)); // yeni kullan�c�n�n puanlamama say�s�
	    int * index_of_zeros = (int*) malloc(n * sizeof(int)); // yeni kullan�c�n�n puanlamad��� indisler
	    
	    find_newuser(stream, new_user, newuser_points, n, index_of_zeros, num_of_zeros);
	    
		for(j = 0; j < U; j++){
			double pearson_coeff = calc(newuser_points, user_matrix[j], n); // pearson katsayisi hesaplan�yor
			newuser_similarity[j] = pearson_coeff;
		}
		for(j = 0; j < k; j++){
			find_max(newuser_similarity, index, similarity, j, U); // en y�ksek benzerli�e sahip kullan�c� bulunuyor
			printf(" (%d). reader : %s - (%.5f)\n", j+1, users[*index], newuser_similarity[*index]);
			index_of_users[j] = *index;
			newuser_similarity[*index] = -2; // bulunan en y�kse�i devreden ��kartmak i�in
		}
		double max = -1;
		int predicted_book_index;
		printf(" Estimated liking for unread books \n");
		for(j = 0; j < *num_of_zeros; j++){
			double guess = estimate(index_of_users, index_of_zeros[j], k, similarity, newuser_points, n, user_matrix);
			if(guess > max){ // en y�ksek oylu kitap indisi aran�yor
				max = guess;
				predicted_book_index = index_of_zeros[j];
			}
			printf(" (%s) : (%.5f) \n", books[index_of_zeros[j]], guess);
		}
		printf(" Recommended book for New User (%s) : (%s) \n", new_user, books[predicted_book_index]);
		
		printf(" Do you want to continue? (Yes: 1 No: Any number) " );
		scanf("%d", &cont);
		printf("***************************************************\n");
		
	}while(cont == 1);
		
    return 0;
}
