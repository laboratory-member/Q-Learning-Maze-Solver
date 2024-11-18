import tkinter as tk
from tkinter import messagebox
import numpy as np
import random
import time


class MazeEnv:
    def __init__(self, size=10, obstacle_prob=0.2):
        self.size = size
        self.maze = np.zeros((size, size))
        self.start = (0, 0)
        self.goal = (size - 1, size - 1)

        # 随机生成障碍
        np.random.seed(42)
        for i in range(size):
            for j in range(size):
                if (i, j) != self.start and (i, j) != self.goal:
                    self.maze[i, j] = 1 if np.random.rand() < obstacle_prob else 0

        # 确保路径通畅
        self._ensure_path()
        self.reset()

    def _ensure_path(self):
        for i in range(self.size):
            self.maze[i, int(self.size/2)] = 0
        for j in range(int(self.size/2)):
            self.maze[0, j] = 0
        for j in range(int(self.size/2)+1,self.size):
            self.maze[self.size - 1, j] = 0

    def reset(self):
        self.agent_pos = self.start
        return self.agent_pos

    def step(self, action):
        x, y = self.agent_pos
        if action == 0:  # 上
            x = max(x - 1, 0)
        elif action == 1:  # 下
            x = min(x + 1, self.size - 1)
        elif action == 2:  # 左
            y = max(y - 1, 0)
        elif action == 3:  # 右
            y = min(y + 1, self.size - 1)

        if self.maze[x, y] == 1:
            reward = -1
            done = False
        elif (x, y) == self.goal:
            reward = 10
            done = True
        else:
            reward = -0.1
            done = False

        self.agent_pos = (x, y)
        return self.agent_pos, reward, done


class MazeApp:
    def __init__(self):
        self.root = tk.Tk()
        self.root.title("Q-Learning Maze Solver")

        # 输入界面
        self.input_frame = tk.Frame(self.root)
        self.input_frame.pack(pady=10)

        tk.Label(self.input_frame, text="Maze Size (5-50): ").grid(row=0, column=0)
        self.size_entry = tk.Entry(self.input_frame)
        self.size_entry.grid(row=0, column=1)

        tk.Label(self.input_frame, text="Training Episodes (100-5000): ").grid(row=1, column=0)
        self.episodes_entry = tk.Entry(self.input_frame)
        self.episodes_entry.grid(row=1, column=1)

        tk.Label(self.input_frame, text="Obstacle Density: ").grid(row=2, column=0)
        self.obstacle_var = tk.StringVar(value="Medium")
        tk.Radiobutton(self.input_frame, text="Low", variable=self.obstacle_var, value="Low").grid(row=2, column=1)
        tk.Radiobutton(self.input_frame, text="Medium", variable=self.obstacle_var, value="Medium").grid(row=2, column=2)
        tk.Radiobutton(self.input_frame, text="High", variable=self.obstacle_var, value="High").grid(row=2, column=3)

        self.start_button = tk.Button(self.root, text="Start Training", command=self.start)
        self.start_button.pack(pady=10)

        self.canvas = tk.Canvas(self.root, width=600, height=600, bg="white")
        self.status_label = tk.Label(self.root, text="", font=("Arial", 14))
        self.train_button = None

        # 子窗口：Q 表显示
        self.q_table_window = None
        self.q_text = None

    def start(self):
        # 获取用户输入
        try:
            maze_size = int(self.size_entry.get())
            train_episodes = int(self.episodes_entry.get())
            obstacle_density = self.obstacle_var.get()

            if not (5 <= maze_size <= 50):
                raise ValueError("Maze size must be between 5 and 50.")
            if not (100 <= train_episodes <= 5000):
                raise ValueError("Training episodes must be between 100 and 5000.")

            # 设置障碍密度
            obstacle_prob = {"Low": 0.1, "Medium": 0.2, "High": 0.3}[obstacle_density]
            self.env = MazeEnv(size=maze_size, obstacle_prob=obstacle_prob)
            self.q_table = np.zeros((maze_size * maze_size, 4))

            # 清空旧的界面
            self.input_frame.destroy()
            self.start_button.destroy()
            self.canvas.pack()
            self.status_label.pack()

            # 创建 Q 表窗口
            self.q_table_window = tk.Toplevel(self.root)
            self.q_table_window.title("Q-Table Viewer")
            self.q_text = tk.Text(self.q_table_window, width=70, height=30)
            self.q_text.pack()

            # 开始训练
            self.train(train_episodes)

        except ValueError as e:
            messagebox.showerror("Input Error", str(e))

    def draw_maze(self):
        self.canvas.delete("all")
        for i in range(self.env.size):
            for j in range(self.env.size):
                x1, y1 = j * (600 // self.env.size), i * (600 // self.env.size)
                x2, y2 = x1 + (600 // self.env.size), y1 + (600 // self.env.size)
                if self.env.maze[i, j] == 1:
                    self.canvas.create_rectangle(x1, y1, x2, y2, fill="black")
                elif (i, j) == self.env.start:
                    self.canvas.create_rectangle(x1, y1, x2, y2, fill="green")
                elif (i, j) == self.env.goal:
                    self.canvas.create_rectangle(x1, y1, x2, y2, fill="red")

    def draw_agent(self, pos):
        x, y = pos
        cell_size = 600 // self.env.size
        x1, y1 = y * cell_size, x * cell_size
        x2, y2 = x1 + cell_size, y1 + cell_size
        self.canvas.create_oval(x1, y1, x2, y2, fill="blue")

    def state_to_index(self, state):
        return state[0] * self.env.size + state[1]

    def update_q_table_view(self):
        """更新 Q 表的显示内容"""
        self.q_text.delete(1.0, tk.END)
        for state in range(self.q_table.shape[0]):
            row, col = divmod(state, self.env.size)
            q_values = self.q_table[state]
            self.q_text.insert(tk.END, f"   State ({row}, {col}): {q_values}\n")
        self.q_text.see(tk.END)  # 自动滚动到最后

    def train(self, episodes):
        alpha = 0.1
        gamma = 0.95
        epsilon = 1.0
        epsilon_decay = 0.995
        epsilon_min = 0.1

        for episode in range(episodes):
            state = self.env.reset()
            state_index = self.state_to_index(state)
            done = False

            while not done:
                self.draw_maze()
                self.draw_agent(state)
                self.root.update()
                time.sleep(0.01)

                # 显示训练进度
                self.status_label.config(text=f"Training: Episode {episode + 1}/{episodes}")
                self.status_label.update()

                if random.random() < epsilon:
                    action = random.randint(0, 3)
                else:
                    action = np.argmax(self.q_table[state_index])

                next_state, reward, done = self.env.step(action)
                next_state_index = self.state_to_index(next_state)

                best_next_action = np.argmax(self.q_table[next_state_index])
                self.q_table[state_index, action] += alpha * (
                    reward + gamma * self.q_table[next_state_index, best_next_action] - self.q_table[state_index, action]
                )

                state = next_state
                state_index = next_state_index

            if epsilon > epsilon_min:
                epsilon *= epsilon_decay

            # 更新 Q 表窗口
            self.update_q_table_view()

        self.status_label.config(text="Training complete! Displaying shortest path...")
        self.test()

    def test(self):
        state = self.env.reset()
        path = [state]
        done = False

        while not done:
            self.draw_maze()
            self.draw_agent(state)
            self.root.update()
            time.sleep(0.2)

            state_index = self.state_to_index(state)
            action = np.argmax(self.q_table[state_index])
            state, _, done = self.env.step(action)
            path.append(state)

        self.draw_maze()
        for pos in path:
            self.draw_agent(pos)
            self.root.update()
            time.sleep(0.2)

        self.status_label.config(text="Shortest path displayed!")

    def run(self):
        self.root.mainloop()


if __name__ == "__main__":
    app = MazeApp()
    app.run()
