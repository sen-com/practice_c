#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include <time.h>

#define N 100                               //遺伝子数
#define SIZE_OF_GENE_POOL 100             //遺伝子ブールのサイズ
#define LAST_GENERATION 1000                //繰り返しを打ち切る世代
#define PROBABILITY_OF_MUTATION 0.01      //突然変異が起こる確率


void init_gene_pool(int gene_pool[SIZE_OF_GENE_POOL][N]); //遺伝子プールの初期化
int range_rand();                                         //n未満の乱数の生成
void crossover(int gene_pool[SIZE_OF_GENE_POOL][N], int next_gene_pool[SIZE_OF_GENE_POOL*2][N], int expected_value[N]);   //交叉
void mutation(int next_gene_pool[SIZE_OF_GENE_POOL*2][N]);//突然変異
int calculating_fitness(int gene_pool[], int expected_value[]); //適応度の計算
int selecting_parent(int fitness[SIZE_OF_GENE_POOL],int total_fitness);//親を選択
void generating_children(int father[], int mother[], int childlen1[], int childlen2[]);//子の生成
void selecting_next_generation(int gene_pool[SIZE_OF_GENE_POOL][N], int next_gene_pool[SIZE_OF_GENE_POOL*2][N], int expected_value[N]);  //次世代の選択
void print_gene_pool(int gene_pool[SIZE_OF_GENE_POOL][N], int expected_value[N]);//遺伝子情報の出力


int main() {
  int gene_pool[SIZE_OF_GENE_POOL][N];       //現世代の染色体の遺伝子プール
  int next_gene_pool[SIZE_OF_GENE_POOL*2][N];//次世代の染色体の遺伝子プール
  int generation;                            //現在の世代
  int expected_value[N];                  //期待値


  srand((unsigned) time(NULL));

  init_gene_pool(gene_pool);              //遺伝子プールの初期化

  //printf("期待値\n");
  for(int i=0; i<N; ++i) {                //期待値を生成
    expected_value[i] = range_rand(2);
    //printf("%1d",expected_value[i]);   //期待値の出力
  }
  printf("\n");

  for(generation=0; generation<LAST_GENERATION; ++generation) {
    //printf("第%d世代\n",generation+1);                      //世代の出力
    crossover(gene_pool, next_gene_pool, expected_value);   //交叉
    mutation(next_gene_pool);                               //突然変異
    selecting_next_generation(gene_pool, next_gene_pool, expected_value); //次世代の選択
    print_gene_pool(gene_pool, expected_value);             //現世代の出力
  }
  return 0;
}

/*  遺伝子プールの初期化  */
void init_gene_pool(int gene_pool[SIZE_OF_GENE_POOL][N]) {
  for(int i=0; i<SIZE_OF_GENE_POOL; ++i) {
    for(int j=0; j<N; ++j) {
      gene_pool[i][j] = range_rand(2);
    }
  }
}

/*  指定された範囲の乱数を生成  */
int range_rand(int range){
  int random;             //生成する乱数
  random = rand()%range;  //乱数を生成
  return random;          //生成した乱数を返す
}

/*  交叉  */
void crossover(int gene_pool[SIZE_OF_GENE_POOL][N], int next_gene_pool[SIZE_OF_GENE_POOL*2][N], int expected_value[N]) {
  int fitness[SIZE_OF_GENE_POOL];   //適応度
  int total_fitness=0;               //適応度の合計
  int father,mother;                //親の遺伝子
  /*  ルーレット選択  */
  for(int i=0; i<SIZE_OF_GENE_POOL; ++i) {
    fitness[i] = calculating_fitness(gene_pool[i], expected_value);   //適応度を格納
    total_fitness += fitness[i];          //適応度の合計を計算
  }

  for(int i=0; i<SIZE_OF_GENE_POOL; ++i) {//2人の親から2人の子を作り出す動作を遺伝子ブールのサイズの回数繰り返す。できる子は遺伝子プールサイズの2倍
    do {
      /* 親を選択 */
      father = selecting_parent(fitness, total_fitness);
      mother = selecting_parent(fitness, total_fitness);
    }while(father == mother);             //親がかぶったら選択しなおす。

    //子の生成
    generating_children(gene_pool[father], gene_pool[mother], next_gene_pool[i*2], next_gene_pool[i*2+1]);
  }
}

/*  適応度の計算  */
int calculating_fitness(int gene_pool[], int expected_value[]) {
  int fitness=0;      //適応度
  int locus=0;        //遺伝子座
  for(locus; locus<N; ++locus) {
    if(gene_pool[locus]==expected_value[locus]){
      fitness++;
    }
  }
  return fitness;
}

/* 親の選択 */
int selecting_parent(int fitness[SIZE_OF_GENE_POOL],int total_fitness) {
    int fit_rand = range_rand(total_fitness);    //適応度の合計以下の値をランダムに生成
    int add_fit = 0;                             //適応度を足していく変数
    int i;

    for(i=0; i<SIZE_OF_GENE_POOL; ++i){
      add_fit += fitness[i];                     //適応度を加算していき、
      if(add_fit > fit_rand)break;               //fit_randの値を超えたらbreak
    }
    return i;                                    //超えたときのiを親の遺伝子番号として返す
}

/* 子の生成 */
void generating_children(int father[], int mother[], int childlen1[], int childlen2[]){
  int i;
  int crrossing_point = range_rand(N);   //交叉点を決定

  //前半部分のコピー
  for(i=0; i<crrossing_point; ++i) {
    childlen1[i] = father[i];
    childlen2[i] = mother[i];
  }
  //後半部分のコピー
  for(; i<N; ++i) {
    childlen1[i] = mother[i];
    childlen2[i] = father[i];
  }
}

/*  突然変異  */
void mutation(int next_gene_pool[SIZE_OF_GENE_POOL*2][N]){

  for(int i=0; i<SIZE_OF_GENE_POOL; ++i) {
    if((double)range_rand(100)/100.0 < PROBABILITY_OF_MUTATION){//0.01の確率で突然変異を起こす
    //printf("突然変異が起こりました\n");
      for(int j=0; j<N; j++){
        if(next_gene_pool[i][j] == 1) {
          next_gene_pool[i][j] = 0;      //反転操作
        }else if(next_gene_pool[i][j] == 0) {
          next_gene_pool[i][j] = 1; //反転操作
        }
      }
    }
  }
}
/*  次世代の選択  */
void selecting_next_generation(int gene_pool[SIZE_OF_GENE_POOL][N], int next_gene_pool[SIZE_OF_GENE_POOL*2][N], int expected_value[N]) {
  int fitness[SIZE_OF_GENE_POOL*2];   //適応度
  int total_fitness=0;               //適応度の合計
  int tmp;

  /*  ルーレット選択  */
  for(int i=0; i<SIZE_OF_GENE_POOL*2; ++i) {
    fitness[i] = calculating_fitness(gene_pool[i], expected_value);   //適応度を格納
  }

  for (int j=0; j<SIZE_OF_GENE_POOL*2; ++j) {   //昇順ソート
    for (int k=j+1; k<SIZE_OF_GENE_POOL*2; ++k) {
      if (fitness[j] < fitness[k]) {
        for(int m=0; m<N; ++m){
          tmp =  next_gene_pool[j][m];
          next_gene_pool[j][m] = next_gene_pool[k][m];
          next_gene_pool[k][m] = tmp;
        }
      }
    }
  }

  for(int n=0; n<SIZE_OF_GENE_POOL; ++n){       //次世代を昇順ソートしたものを順番にSIZE_OF_GENE_POOLまで現世代に格納
    for(int s=0; s<N; ++s) {
      gene_pool[n][s] = next_gene_pool[n][s];
    }
  }
}

/*  遺伝子プールの出力  */
void print_gene_pool(int gene_pool[SIZE_OF_GENE_POOL][N], int expected_value[N]) {
  int fitness;           //適応度
  double totalfitness=0; //適応度の合計値
  int elite,bestfitness=0;   //エリート遺伝子の処理用変数

  for(int i=0; i<SIZE_OF_GENE_POOL; ++i) {
    for(int j=0; j<N; ++j) {
      //printf("%1d",gene_pool[i][j]);
    }
    fitness = calculating_fitness(gene_pool[i],expected_value);
    //printf("\t%d\n",fitness);
    if(fitness>bestfitness){    //エリート個体の算出
      bestfitness = fitness;
      elite = i;
    }
    totalfitness += fitness;
  }

  //エリート個体の出力
  // printf("エリート個体は");
  // for(int j=0; j<N; j++) {
  //   printf("%d", gene_pool[elite][j]);
  // }
  // printf("\n");

  // printf("期待値は");
  // for(int j=0; j<N; j++) {
  //   printf("%d", expected_value[0][j]);
  // }
  // printf("\n");

  //printf("適応度：%d\n", bestfitness);
  printf("%d\n", bestfitness);

  //平均の適応度を出力
  //printf("平均適応度は%lf\n",totalfitness/(SIZE_OF_GENE_POOL*N)*100);
}
