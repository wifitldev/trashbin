import pygame
import random
import sys
import time

pygame.init()

WIDTH, HEIGHT = 400, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("JUMP")

WHITE = (255, 255, 255)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
BLACK = (0, 0, 0)

jump_sound = pygame.mixer.Sound("sound.wav")

player = pygame.Rect(200, 300, 50, 50)
player_velocity = 0
gravity = 0.5

platforms = [pygame.Rect(random.randint(0, WIDTH - 100), i * 100, 100, 10) for i in range(6)]

platform_speed = 2

score = 0
font = pygame.font.SysFont("Arial", 24)

def show_start_screen():
    screen.fill(WHITE)
    title_font = pygame.font.SysFont("Arial", 36, bold=True)
    title_text = title_font.render("Jump", True, BLUE)
    instructions = font.render("Press any key to start", True, BLACK)
    screen.blit(title_text, (WIDTH // 2 - title_text.get_width() // 2, HEIGHT // 3))
    screen.blit(instructions, (WIDTH // 2 - instructions.get_width() // 2, HEIGHT // 2))
    pygame.display.flip()

    waiting = True
    while waiting:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
            if event.type == pygame.KEYDOWN:
                waiting = False

clock = pygame.time.Clock()
running = True

show_start_screen()

while running:
    screen.fill(WHITE)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT] and player.x > 0:
        player.x -= 5
    if keys[pygame.K_RIGHT] and player.x < WIDTH - player.width:
        player.x += 5

    player_velocity += gravity
    player.y += player_velocity

    if player.y > HEIGHT:
        running = False

    for platform in platforms:
        if player.colliderect(platform) and player_velocity > 0:
            player_velocity = -10
            jump_sound.play()
            break

    for platform in platforms:
        platform.y += platform_speed

        if platform.y > HEIGHT:
            platforms.remove(platform)
            new_platform = pygame.Rect(random.randint(0, WIDTH - 100), -10, 100, 10)
            platforms.append(new_platform)
            score += 10

            platform_speed = 2 + score // 100

    pygame.draw.rect(screen, BLUE, player)
    for platform in platforms:
        pygame.draw.rect(screen, GREEN, platform)
        score_text = font.render(f"Счет: {score}", True, BLACK)
        screen.blit(score_text, (10, 10))
    pygame.display.flip()
    clock.tick(60)

pygame.quit()

