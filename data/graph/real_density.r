#ファイルパスを取得
setwd("/Users/sho/graph");

network <- read.csv("/Users/sho/graph/network.csv", head = T);
observation <- read.csv("/Users/sho/graph/observer_location.csv", head = T);

as.matrix(network);
as.matrix(observation);

for(i in 1:100){
  #ファイルの読み込み
  check <- try(read.csv(paste("/Users/sho/graph/10min/traffic_flow_", ".csv", sep=(as.character(i))), head = T), silent = TRUE);

  if(class(check) != "try-error"){
    data <- check;

    for(j in 1:nrow(observation)){

      if(i == observation[j,1]){

        for(k in 1:nrow(network)){

          if((network[k,1] == observation[j,2]) && (network[k,2] == observation[j,3])){
            data[,2] <- data[,2] / network[k,3];
            break;
          }
        }
        break;
      }
    }
    write.csv(data, paste("/Users/sho/graph/10min/modified_traffic_", ".csv", sep = (as.character(i))));
  }
}
