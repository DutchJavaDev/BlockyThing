#pragma once
static const float hitBoxMultiplier = 1.0f;

static void RunCollisionSimulations(ecs_iter_t* it, WorldPosition* dynamicPositionArray, Velocity* dynamicVelocityArray,
	DynamicBody* dynamicBodyArray, ecs_iter_t static_bodies_iterator)
{
	while (ecs_filter_next(&static_bodies_iterator))
	{
		WorldPosition* staticPositionArray = ecs_field(&static_bodies_iterator, WorldPosition, 1);
		StaticBody* staticBodyArray = ecs_field(&static_bodies_iterator, StaticBody, 2);

		for (size_t ii = 0; ii < static_bodies_iterator.count; ii++)
		{
			WorldPosition* staticPosition = &staticPositionArray[ii];
			StaticBody* staticBody = &staticBodyArray[ii];
			// create a hit box around self
			// +50% of self

			float parentCenterX = staticBody->centerX;
			float parentCenterY = staticBody->centerY;
			float parentWidth = staticBody->baseWidth;
			float parentHeight = staticBody->baseHeight;

			float hitBoxWidth = parentWidth * hitBoxMultiplier;
			float hitBoxHeight = parentHeight * hitBoxMultiplier;
			float hitBoxX = parentCenterX - hitBoxWidth / 2;
			float hitBoxY = parentCenterY - hitBoxHeight / 2;

			Rectangle staticHitbox = { hitBoxX, hitBoxY, hitBoxWidth, hitBoxHeight };

			for (size_t i = 0; i < it->count; i++)
			{
				WorldPosition* dynamicPosition = &dynamicPositionArray[i];
				Velocity* velocity = &dynamicVelocityArray[i];
				DynamicBody dynamicBody = dynamicBodyArray[i];

				Rectangle dynamicHitbox = (Rectangle){ dynamicPosition->x, dynamicPosition->y, dynamicBody.baseWidth, dynamicBody.baseHeight };

				if (CheckCollisionRecs(staticHitbox, dynamicHitbox))
				{
					// start checking if colliding with parent
					//if (CheckCollisionRecs(dynamicHitbox, parentHitbox))
					{
						// center point
						float dynamicCenterX = dynamicHitbox.x + dynamicHitbox.width / 2;
						float dynamicCenterY = dynamicHitbox.y + dynamicHitbox.height / 2;

						// difference between center points
						float deltaX = dynamicCenterX - parentCenterX;
						float deltaY = dynamicCenterY - parentCenterY;

						// Vertical collision (up or down)
						if (abs(deltaX) < abs(deltaY))
						{

							// bottom collision
							if (deltaY < 0 && velocity->yVelocity > 0.0f)
							{
								// snap to position
								dynamicPosition->y = staticHitbox.y - dynamicBody.baseHeight;
								velocity->yVelocity = 0;
								continue;
							}

							// top collision
							if (deltaY > 0 && velocity->yVelocity < 0.0f)
							{
								// snap to position
								dynamicPosition->y = staticHitbox.y + staticHitbox.height;
								velocity->yVelocity = 0;
								continue;
							}

						}

						if (abs(deltaX) > abs(deltaY))
						{
							// right side collision
							if (deltaX < 0 && velocity->xVelocity > 0.0f)
							{
								// snap to position
								dynamicPosition->x = staticHitbox.x - dynamicBody.baseWidth;
								velocity->xVelocity = 0;
								continue;
							}

							// left side collision
							if (deltaX > 0 && velocity->xVelocity < 0.0f)
							{
								// snap to position
								dynamicPosition->x = staticHitbox.x + staticHitbox.width;
								velocity->xVelocity = 0;
								continue;
							}
						}
					}
				}
			}
		}
	}
}