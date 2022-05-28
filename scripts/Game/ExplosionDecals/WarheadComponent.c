class WarheadComponentClass : ScriptComponentClass
{
}

class WarheadComponent : ScriptComponent
{
	private static const int DECAL_COLOR = Color.FromRGBA(255, 255, 255, 255).PackToInt();
	private static const ref array<string> MATERIAL_PATHS =
	{
		"{7517F14CACE7D68A}materials/ExplosionScorchMark1.emat",
		"{096D7015B630FD15}materials/ExplosionScorchMark2.emat",
	};

	override void OnPostInit(IEntity owner)
	{
		World world = owner.GetWorld();
		auto position = owner.GetOrigin() + owner.GetAngles();
		SpawnDecal(owner, world, position, vector.Up);
		SpawnDecal(owner, world, position, -vector.Up);
		SpawnDecal(owner, world, position, vector.Right);
		SpawnDecal(owner, world, position, -vector.Right);

	}
	
	static void SpawnDecal(IEntity owner, World world, vector origin, vector direction)
	{
		vector intersectionPosition;
		auto surfaceTraceParam = GetSurfaceIntersection(
			owner,
            world,
            origin,
            direction,
            2,
			intersectionPosition
        );
        if (surfaceTraceParam.TraceEnt)
        {
			auto decal = world.CreateDecal(
				surfaceTraceParam.TraceEnt,
				intersectionPosition + surfaceTraceParam.TraceNorm * 0.25,
				-surfaceTraceParam.TraceNorm,
				0,
				4,
				Math.RandomFloat(0, 360) * Math.DEG2RAD,
				Math.RandomFloat(4, 8),
				1,
				MATERIAL_PATHS.GetRandomElement(),
				-1,
				DECAL_COLOR
			);
			
			#ifdef WORKBENCH
			Shape.CreateSphere(COLOR_GREEN_A, ShapeFlags.TRANSP | ShapeFlags.NOOUTLINE, intersectionPosition + surfaceTraceParam.TraceNorm * 0.25, 0.25);
			Shape.CreateArrow(intersectionPosition + surfaceTraceParam.TraceNorm, intersectionPosition + surfaceTraceParam.TraceNorm * 2, 0.2, COLOR_RED, ShapeFlags.DEFAULT);
			#endif
        }
	}

	static TraceParam GetSurfaceIntersection(
		IEntity owner,
		World world,
		vector origin,
		vector direction,
		float distance,
		out vector intersectionPosition
	)
	{
		auto param = new TraceParam();
  		param.Start = origin;
  		param.End = origin + direction * distance;
  		param.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
  		param.Exclude = owner;
		float intersectionDistance = world.TraceMove(param, NULL) * distance;
		intersectionPosition = origin + (direction * intersectionDistance);
		return param;
	}
}