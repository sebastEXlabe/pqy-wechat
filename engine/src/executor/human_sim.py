"""类人行为模拟器"""

import asyncio
import random
from typing import Tuple, List
import numpy as np
from loguru import logger


class HumanSimulator:
    """类人行为模拟器"""

    def __init__(self):
        self.mouse_speed_range = (0.3, 1.2)
        self.pause_probability = 0.1
        self.overshoot_range = (2, 5)
        self.wpm_range = (40, 80)
        self.typo_probability = 0.02

    def generate_bezier_curve(
        self,
        start: Tuple[int, int],
        end: Tuple[int, int],
        num_points: int = 50
    ) -> List[Tuple[int, int]]:
        """生成贝塞尔曲线轨迹"""
        # 控制点（添加随机偏移）
        control1 = (
            start[0] + (end[0] - start[0]) * 0.3 + random.randint(-20, 20),
            start[1] + (end[1] - start[1]) * 0.3 + random.randint(-20, 20)
        )
        control2 = (
            start[0] + (end[0] - start[0]) * 0.7 + random.randint(-20, 20),
            start[1] + (end[1] - start[1]) * 0.7 + random.randint(-20, 20)
        )

        points = []
        for t in np.linspace(0, 1, num_points):
            # 三阶贝塞尔曲线
            x = (1-t)**3 * start[0] + 3*(1-t)**2*t * control1[0] + \
                3*(1-t)*t**2 * control2[0] + t**3 * end[0]
            y = (1-t)**3 * start[1] + 3*(1-t)**2*t * control1[1] + \
                3*(1-t)*t**2 * control2[1] + t**3 * end[1]
            points.append((int(x), int(y)))

        return points

    def get_move_duration(self) -> float:
        """获取移动持续时间（Gamma 分布）"""
        return np.random.gamma(shape=2.0, scale=0.3)

    def get_typing_delay(self) -> float:
        """获取打字延迟（模拟真实打字节奏）"""
        wpm = random.uniform(*self.wpm_range)
        return 60.0 / (wpm * 5)

    async def random_delay(self, min_ms: int = 100, max_ms: int = 500):
        """随机延迟"""
        delay = random.uniform(min_ms / 1000, max_ms / 1000)
        await asyncio.sleep(delay)

    async def human_click(self, x: int, y: int):
        """类人点击"""
        try:
            import pyautogui

            # 获取当前位置
            current_x, current_y = pyautogui.position()

            # 生成轨迹
            points = self.generate_bezier_curve(
                (current_x, current_y),
                (x, y)
            )

            # 移动鼠标
            duration = self.get_move_duration()
            for point in points:
                pyautogui.moveTo(point[0], point[1], _pause=False)
                await asyncio.sleep(duration / len(points))

            # 随机停顿
            if random.random() < self.pause_probability:
                await asyncio.sleep(random.uniform(0.1, 0.3))

            # 点击
            pyautogui.click()

        except ImportError:
            logger.warning("pyautogui not available, skipping human simulation")
            # 直接使用 UIAutomation 点击
            pass

    async def human_type(self, text: str):
        """类人打字"""
        try:
            import pyautogui

            for char in text:
                pyautogui.write(char, interval=0)
                await asyncio.sleep(self.get_typing_delay())

                # 随机打字错误
                if random.random() < self.typo_probability:
                    wrong_char = chr(ord(char) + random.randint(-2, 2))
                    pyautogui.write(wrong_char, interval=0)
                    await asyncio.sleep(self.get_typing_delay())
                    pyautogui.press('backspace')
                    await asyncio.sleep(self.get_typing_delay())

        except ImportError:
            logger.warning("pyautogui not available, skipping human simulation")
            pass

    async def random_extra_action(self):
        """随机多余动作"""
        if random.random() < 0.05:
            try:
                import pyautogui
                if random.random() < 0.5:
                    pyautogui.scroll(random.randint(-3, 3))
                else:
                    x = random.randint(100, 1800)
                    y = random.randint(100, 900)
                    await self.human_click(x, y)
            except ImportError:
                pass


# 全局实例
human_sim = HumanSimulator()
