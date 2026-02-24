import pygame

pygame.init()

WIDTH, HEIGHT = 800, 600
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("platforms")

WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
BLUE = (0, 0, 255)
GREEN = (0, 255, 0)
BROWN = (139, 69, 19)

FPS = 60
clock = pygame.time.Clock()

player_width = 50
player_height = 50
player_x = 100
player_y = 500
player_speed = 5
player_jump = 15
player_color = BLUE
player_vel_y = 0
is_jumping = False

gravity = 0.8

locations = [
    {
        "platforms": [
            pygame.Rect(0, 550, 800, 50),
            pygame.Rect(200, 450, 150, 20),
            pygame.Rect(400, 350, 200, 20),
            pygame.Rect(650, 250, 100, 20),
        ],
        "flag": pygame.Rect(750, 200, 30, 100),
        "enemy": pygame.Rect(500, 510, 40, 40)
    },
    {
        "platforms": [
            pygame.Rect(0, 550, 800, 50),
            pygame.Rect(150, 400, 200, 20),
            pygame.Rect(450, 300, 150, 20),
            pygame.Rect(600, 200, 100, 20),
        ],
        "flag": pygame.Rect(750, 150, 30, 100),
        "enemy": pygame.Rect(300, 510, 40, 40)
    },
    {
        "platforms": [
            pygame.Rect(0, 550, 800, 50),
            pygame.Rect(200, 450, 150, 20),
            pygame.Rect(400, 325, 200, 20),
            pygame.Rect(650, 250, 456 , 20),
        ],
        "flag": pygame.Rect(750, 200, 30, 100),
        "enemy": pygame.Rect(500, 510, 40, 40)
    },
]

current_location = 0
platforms = locations[current_location]["platforms"]
flag = locations[current_location]["flag"]
enemy = locations[current_location]["enemy"]
enemy_speed = 2

running = True
while running:
    screen.fill(WHITE)
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    keys = pygame.key.get_pressed()

    if keys[pygame.K_LEFT]:
        player_x -= player_speed
    if keys[pygame.K_RIGHT]:
        player_x += player_speed

    if keys[pygame.K_SPACE] and not is_jumping:
        player_vel_y = -player_jump
        is_jumping = True

    player_vel_y += gravity
    player_y += player_vel_y

    if player_x < 0:
        player_x = 0
    if player_x + player_width > WIDTH:
        player_x = WIDTH - player_width

    player_rect = pygame.Rect(player_x, player_y, player_width, player_height)
    for platform in platforms:
        if player_rect.colliderect(platform) and player_vel_y > 0:
            player_y = platform.top - player_height
            player_vel_y = 0
            is_jumping = False

    if player_rect.colliderect(enemy):
        print("Вы проиграли!")
        running = False

    if player_rect.colliderect(flag):
        current_location = (current_location + 1) % len(locations)
        platforms = locations[current_location]["platforms"]
        flag = locations[current_location]["flag"]
        enemy = locations[current_location]["enemy"]
        player_x, player_y = 100, 500
        player_vel_y = 0
        is_jumping = False

    enemy.x += enemy_speed
    if enemy.x <= 0 or enemy.x + enemy.width >= WIDTH:
        enemy_speed = -enemy_speed

    pygame.draw.rect(screen, player_color, player_rect)

    for platform in platforms:
        pygame.draw.rect(screen, GREEN, platform)

    pygame.draw.rect(screen, BLACK, flag)

    pygame.draw.rect(screen, BROWN, enemy)

    pygame.display.flip()
    clock.tick(FPS)

pygame.quit()