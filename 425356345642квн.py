import pygame
import math
import random

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("бросай мяч в корзину")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)

ball = pygame.Rect(random.randint(50, WIDTH // 2), HEIGHT - 50, 20, 20)
basket = pygame.Rect(WIDTH - 100, HEIGHT - 150, 60, 20)
ball_speed = [0, 0]
ball_in_air = False
power = 0
angle = 45
power_increment = 0.5
max_power = 50
score = 0
attempts = 0

font = pygame.font.Font(None, 36)
gravity = 0.1

running = True
while running:
    screen.fill(WHITE)

    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            if event.key == pygame.K_UP and not ball_in_air:
                angle = min(90, angle + 5)
            elif event.key == pygame.K_DOWN and not ball_in_air:
                angle = max(0, angle - 5)

    keys = pygame.key.get_pressed()
    if keys[pygame.K_SPACE] and not ball_in_air:
        power = min(max_power, power + power_increment)
    elif not keys[pygame.K_SPACE] and power > 0 and not ball_in_air:
        rad_angle = math.radians(angle)
        ball_speed[0] = power * math.cos(rad_angle)
        ball_speed[1] = -power * math.sin(rad_angle)
        ball_in_air = True
        power = 0
        attempts += 1
    if ball_in_air:
        ball.x += ball_speed[0]
        ball.y += ball_speed[1]
        ball_speed[1] += gravity
    if ball.bottom >= HEIGHT or ball.left > WIDTH or ball.right < 0:
        ball_in_air = False
        ball_speed = [0, 0]
        ball.x = random.randint(50, WIDTH // 2)
        ball.y = HEIGHT - 50

    pygame.draw.rect(screen, RED, basket)
    pygame.draw.ellipse(screen, BLUE, ball)

    if not ball_in_air:
        angle_rad = math.radians(angle)
        line_x = ball.x + 100 * math.cos(angle_rad)
        line_y = ball.y - 100 * math.sin(angle_rad)

        pygame.draw.line(screen, (190, 190, 190), (ball.x, ball.y), (line_x, line_y), 5)
        adjusted_line_x = ball.x + (line_x - ball.x) * (power / max_power)
        adjusted_line_y = ball.y - (ball.y - line_y) * (power / max_power)
        pygame.draw.line(screen, BLACK, (ball.x, ball.y), (adjusted_line_x, adjusted_line_y), 5)

    score_text = font.render(f"Попадания: {score}", True, BLACK)
    screen.blit(score_text, (WIDTH - 200, 10))

    attempts_text = font.render(f"Попытки: {attempts}", True, BLACK)
    screen.blit(attempts_text, (WIDTH - 200, 40))
    pygame.display.flip()
    pygame.time.Clock().tick(60)

pygame.quit()