## 10分間の密度の平均と分散を計算してデータを出力しグラフを描画
# ファイルパスを取得
setwd("./")

for(i in 0:273){
  file_path <-  "./../result/density/sctm_result_density_"
  file_name <- paste(file_path, ".csv", sep = as.character(i))
  input <- read.csv(file_name, head=T)
  input <- input[,-1]
  data <- matrix(,nrow=0, ncol=ncol(input))
  sum <- matrix(0,nrow=1, ncol=ncol(input))
  
  count_10min <- 0
  count_4sec <- 0
  count_over <- 0

  for(j in 1:nrow(input)){

    if (count_4sec == 150){ # 10分間の和をとった場合
      sum <- sum + input[j,]
      sum <- sum / matrix(150, nrow=1, ncol=ncol(input))
      data <- rbind(data, sum)
      count_10min <- count_10min + 1

      count_4sec <- 0
      sum <- c(rep(0, length=ncol(input)))

    } else { # 10分間の和がとれていない場合

      if(count_10min < 12){ # 10分間のデータを13回とっていない場合
        sum <- sum + input[j,]
        count_4sec <- count_4sec + 1

      } else { # 130分間のデータを13回すでにとっている場合
        sum <- sum + input[j,]
        count_over <-  count_over + 1

        if(j == nrow(input)){ # データの末尾の処理
          sum <- sum / matrix(count_over, nrow=1, ncol=ncol(input))
          data <- rbind(data, sum)
        }
      }
    }
  }
  
  time <- matrix(1:13, nrow=13, ncol=1) * 10
  data <- cbind(time, data)
  names(data) <- c("Time", "Mean", "Plus", "Minus", "SD")

  # データの保存
  data_file <- "./10min_density/data/sctm_result_10min_density_"
  data_file_name <- paste(data_file, ".csv", sep = as.character(i))
  write.csv(data, data_file_name, row.names=F )

  # グラフの描画と保存
  img_file <- "./10min_density/img/10min_density_mean_variance_"
  img_file_name <- paste(img_file, ".png", sep = as.character(i))
  png(file = img_file_name)
  
  plot(1:13, data$Mean, xlab="時間", ylab="密度", type = "l", col="blue", xlim=c(0,13), ylim=c(0,0.1)); par(new=T)
  plot(1:13, data$Plus, xlab="", ylab="", col="red", type = "l", xlim=c(0,13), ylim=c(0,0.1)); par(new=T)
  plot(1:13, data$Minus, xlab="", ylab="", col="green", type = "l", xlim=c(0,13), ylim=c(0,0.1)); par(new=T)
  legend("topright", legend=c("Mean", "Mean+SD", "Mean-SD"), col=c("blue", "red", "green"), lty=c(1,1,1))
  
  dev.off()
}
