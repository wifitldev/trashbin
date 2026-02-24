import pygame
import sys

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("1234567890-")

BLACK = (0, 0, 0)
WHITE = (255, 255, 255)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)
YELLOW = (255, 255, 0)
PURPLE = (128, 0, 128)

colors = [BLACK, RED, GREEN, BLUE]
current_color_index = 0
current_color = colors[current_color_index]

drawing = False
start_pos = None
shape = "line"
brush_size = 5

canvas = pygame.Surface((WIDTH, HEIGHT))
canvas.fill(WHITE)
font = pygame.font.Font(None, 36)

def draw_text(text, x, y):
    text_surface = font.render(text, True, BLACK)
    screen.blit(text_surface, (x, y))

while True:
    screen.fill(WHITE)
    screen.blit(canvas, (0, 0))

    draw_text(f"Инструмент: {shape}", 10, 10)
    draw_text(f"Цвет: {current_color}", 10, 50)
    draw_text("Нажмите С для очистки, Т для смены , пробел для смены цвета", 10, 90)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

        if event.type == pygame.MOUSEBUTTONDOWN:
            drawing = True
            start_pos = event.pos

        if event.type == pygame.MOUSEBUTTONUP:
            drawing = False
            end_pos = event.pos
            if shape == "line":
                pygame.draw.line(canvas, current_color, start_pos, end_pos, 3)
            elif shape == "rect":
                rect = pygame.Rect(min(start_pos[0], end_pos[0]),
                                   min(start_pos[1], end_pos[1]),
                                   abs(start_pos[0] - end_pos[0]),
                                   abs(start_pos[1] - end_pos[1]))
                pygame.draw.rect(canvas, current_color, rect, 3)
            elif shape == "circle":
                radius = int(((end_pos[0] - start_pos[0]) ** 2 + (end_pos[1] - start_pos[1]) ** 2) ** 0.5)
                pygame.draw.circle(canvas, current_color, start_pos, radius, 3)

        if event.type == pygame.MOUSEMOTION and drawing:
            if shape == "brush":
                pygame.draw.circle(canvas, current_color, event.pos, brush_size)

        if event.type == pygame.KEYDOWN:
            if event.key == pygame.K_c:
                canvas.fill(WHITE)
            if event.key == pygame.K_t:
                if shape == "line":
                    shape = "rect"
                elif shape == "rect":
                    shape = "circle"
                elif shape == "circle":
                    shape = "brush"
                elif shape == "brush":
                    shape = "line"

            if event.key == pygame.K_SPACE:
                current_color_index = (current_color_index + 1) % len(colors)
                current_color = colors[current_color_index]
            if event.key == pygame.K_UP:
                brush_size += 1
            if event.key == pygame.K_DOWN:
                brush_size = max(1, brush_size - 1)

    if drawing and start_pos and shape != "brush":
        mouse_pos = pygame.mouse.get_pos()
        if shape == "line":
            pygame.draw.line(screen, current_color, start_pos, mouse_pos, 3)
        elif shape == "rect":
            rect = pygame.Rect(min(start_pos[0], mouse_pos[0]),
                               min(start_pos[1], mouse_pos[1]),
                               abs(start_pos[0] - mouse_pos[0]),
                               abs(start_pos[1] - mouse_pos[1]))
            pygame.draw.rect(screen, current_color, rect, 3)
        elif shape == "circle":
            radius = int(((mouse_pos[0] - start_pos[0]) ** 2 + (mouse_pos[1] - start_pos[1]) ** 2) ** 0.5)
            pygame.draw.circle(screen, current_color, start_pos, radius, 3)

    pygame.display.flip()





