library(ggplot2)

data <- read.csv("C:/Users/kanis/OneDrive/Documents/Desktop/value/leaderboard_research/results/benchmark.csv")

# Graph 1: Insert time
p1 <- ggplot(data, aes(x=N, y=insert_ms, color=structure, group=structure)) +
  geom_line(size=1.2) +
  geom_point(size=3) +
  labs(
    title="BST vs AVL: Insertion Time",
    x="Number of Users",
    y="Time (milliseconds)",
    color="Structure"
  ) +
  theme_minimal()

# Graph 2: Kth Max query time
p2 <- ggplot(data, aes(x=N, y=kthmax_us, color=structure, group=structure)) +
  geom_line(size=1.2) +
  geom_point(size=3) +
  labs(
    title="BST vs AVL: Kth Max Query Time",
    x="Number of Users",
    y="Time (microseconds)",
    color="Structure"
  ) +
  theme_minimal()

# Save both
ggsave("C:/Users/kanis/OneDrive/Documents/Desktop/value/leaderboard_research/results/insert_graph.png", p1, width=8, height=5)
ggsave("C:/Users/kanis/OneDrive/Documents/Desktop/value/leaderboard_research/results/query_graph.png", p2, width=8, height=5)

print("Graphs saved!")