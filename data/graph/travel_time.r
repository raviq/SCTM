#平均を算出
meanDataTable <- function(frame) {

  sum <- 0
  count <- 0
  origin <- NULL
  destination <- NULL
  mean <- matrix(nrow = 0, ncol = 3)
  frame <- data.matrix(frame)

  for (i in 1:nrow(frame)) {

    if (is.null(origin) && is.null(destination)) {
      origin <- frame[i,1]
      destination <- frame[i,2]
      sum <- frame[i,3]
      count <-  count + 1
    } else if(origin == frame[i,1] && destination == frame[i,2]) {
      sum <- sum + frame[i,3]
      count <- count + 1
    } else {
      mean <- rbind(mean, c(origin, destination, sum / count))

      origin <- frame[i,1]
      destination <- frame[i,2]
      sum <- frame[i,3]
      count <- 1
    }

  }

  mean <- rbind(mean, c(origin, destination, sum / count))
  return (mean)
}

#ファイル読み込み
sctm <- read.csv("./../result/sctm_result_move_time.csv", head=T);
real <- read.csv("./result_trip.csv", head=T);

#余分な列を削除
sctm <- cbind(sctm[,2:3],sctm[,ncol(sctm)])
real <- cbind(real[,1:2],real[,ncol(real)])

#列にラベルを付ける
names(sctm) <- c("Origin", "Destination", "sctm")
names(real) <- c("Origin", "Destination", "real")

#昇順にソート
uporder <- order(sctm$Origin, sctm$Destination)
sctm <- sctm[uporder, ]

uporder <- order(real$Origin, real$Destination)
real <- real[uporder, ]

#所要時間が20分以上のデータを削除
sctm <- subset(sctm, sctm<20)
real <- subset(real, real<20)

#所要時間が1分以下のデータを削除
sctm <- subset(sctm, sctm>=1)
real <- subset(real, real>=1)

#平均を算出
meansctm <- meanDataTable(sctm)
meanreal <- meanDataTable(real)

#行列をデータフレームに変換
sctmframe <- data.frame(meansctm)
realframe <- data.frame(meanreal)

#列にラベルを付ける
names(sctmframe) <- c("Origin", "Destination", "sctm")
names(realframe) <- c("Origin", "Destination", "real")

#データフレームを結合
result <- merge(sctmframe, realframe)

#描画
png(file = "travel_time.png")

plot(result$real, result$sctm, xlab="measured value", ylab="SCTM", col="blue", pch=1, xlim=c(0,20), ylim=c(0,20)); par(new=T)
plot(0:20, 0:20, type="l", xlab="", ylab="")

#相関係数を計算
print(cor(result$real, result$sctm, method="kendall"))
print(cor(result$real, result$sctm, method="spearman"))

#切片0の回帰係数を計算
lm(result$sctm~0+result$real, data=result)
dev.off()
