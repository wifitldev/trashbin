questions = {
    "Сколько планет в Солнечной системе?": "8",
    "Столица Франции?": "Париж"
}

score = 0

for q, a in questions.items():
    user_answer = input(q + " ")
    if user_answer.lower() == a.lower():
        print("Правильно!")
        score += 1
    else:
        print("Неверно!")


print(f"result: {score} из {len(questions)}")