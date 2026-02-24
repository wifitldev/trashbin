import tkinter as tk
from tkinter import font
import math

class TimerApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Таймер")
        self.root.geometry("600x600")
        self.root.configure(bg="#2E3440")

        self.time_left = 60
        self.running = False
        self.custom_font = font.Font(family="Helvetica", size=40, weight="bold")
        self.small_font = font.Font(family="Helvetica", size=20, weight="bold")

        self.label = tk.Label(root, text=self.format_time(self.time_left), font=self.custom_font, fg="#D8DEE9", bg="#2E3440")
        self.label.pack(pady=20)

        self.minutes_frame = tk.Frame(root, bg="#2E3440")
        self.minutes_frame.pack(pady=10)

        self.minutes_label = tk.Label(self.minutes_frame, text="Минуты:", font=self.small_font, fg="#D8DEE9", bg="#2E3440")
        self.minutes_label.pack(side=tk.LEFT, padx=10)

        self.minus_minutes_button = tk.Button(self.minutes_frame, text="-", font=self.small_font, command=lambda: self.adjust_time(-60), bg="#4C566A", fg="#D8DEE9")
        self.minus_minutes_button.pack(side=tk.LEFT, padx=5)

        self.plus_minutes_button = tk.Button(self.minutes_frame, text="+", font=self.small_font, command=lambda: self.adjust_time(60), bg="#4C566A", fg="#D8DEE9")
        self.plus_minutes_button.pack(side=tk.LEFT, padx=5)

        self.seconds_frame = tk.Frame(root, bg="#2E3440")
        self.seconds_frame.pack(pady=10)

        self.seconds_label = tk.Label(self.seconds_frame, text="Секунды:", font=self.small_font, fg="#D8DEE9", bg="#2E3440")
        self.seconds_label.pack(side=tk.LEFT, padx=10)

        self.minus_seconds_button = tk.Button(self.seconds_frame, text="-", font=self.small_font, command=lambda: self.adjust_time(-1), bg="#4C566A", fg="#D8DEE9")
        self.minus_seconds_button.pack(side=tk.LEFT, padx=5)

        self.plus_seconds_button = tk.Button(self.seconds_frame, text="+", font=self.small_font, command=lambda: self.adjust_time(1), bg="#4C566A", fg="#D8DEE9")
        self.plus_seconds_button.pack(side=tk.LEFT, padx=5)

        self.start_button = tk.Button(root, text="Старт", command=self.start_timer, bg="#4C566A", fg="#D8DEE9", font=("Helvetica", 14))
        self.start_button.pack(side=tk.LEFT, padx=20, pady=10)

        self.reset_button = tk.Button(root, text="Сброс", command=self.reset_timer, bg="#4C566A", fg="#D8DEE9", font=("Helvetica", 14))
        self.reset_button.pack(side=tk.RIGHT, padx=20, pady=10)

        self.canvas = tk.Canvas(root, width=200, height=200, bg="#2E3440", highlightthickness=0)
        self.canvas.pack(pady=20)
        self.arrow = self.canvas.create_line(100, 100, 100, 50, width=5, fill="#88C0D0")
        self.angle = 0

    def format_time(self, seconds):
        minutes, seconds = divmod(seconds, 60)
        return f"{minutes:02}:{seconds:02}"

    def adjust_time(self, delta):
        if not self.running:
            self.time_left = max(0, self.time_left + delta)
            self.label.config(text=self.format_time(self.time_left))

    def update_timer(self):
        if self.running and self.time_left > 0:
            self.time_left -= 1
            self.label.config(text=self.format_time(self.time_left))
            self.rotate_arrow()
            self.root.after(1000, self.update_timer)
        elif self.time_left == 0:
            self.running = False
            self.label.config(text="Время вышло!")
            self.canvas.itemconfig(self.arrow, fill="#BF616A")

    def rotate_arrow(self):
        self.angle = (self.angle + 6) % 360
        radians = math.radians(self.angle)
        x = 100 + 50 * math.sin(radians)
        y = 100 - 50 * math.cos(radians)
        self.canvas.coords(self.arrow, 100, 100, x, y)

    def start_timer(self):
        if not self.running:
            self.running = True
            self.update_timer()

    def reset_timer(self):
        self.running = False
        self.time_left = 60
        self.label.config(text=self.format_time(self.time_left))
        self.canvas.itemconfig(self.arrow, fill="#88C0D0")
        self.angle = 0
        self.rotate_arrow()

if __name__ == "__main__":
    root = tk.Tk()
    app = TimerApp(root)
    root.mainloop()