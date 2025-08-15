local tex = nil
local log = nil

-- Position and velocity for bouncing
local pos = Vector2f.new(200, 200)
local vel = Vector2f.new(200, 150)  -- pps
local screenSize = Vector2f.new(window.width, window.height)

-- Background color
local bgColor = Vector3f.new(0.2, 0.3, 0.4)
local targetColor = Vector3f.new(math.random(), math.random(), math.random())
local colorSpeed = 1

local function lerp(a, b, t)
    return a + (b - a) * t
end

function setup()
    window.setTitle("Hello from Lua!")
    log = logger.new("Game")

    tex = texture.load("shrek", "assets/shrek.png")
    if not tex or tex.id == 0 then
        log:error("Failed to load texture!")
        engine.exit()
    end

    -- Scale down by half
    tex.width = tex.width / 2
    tex.height = tex.height / 2

    log:info("Loaded texture: " .. math.floor(tex.width) .. "x" .. math.floor(tex.height))
end

function update(deltaTime)
    pos.x = pos.x + vel.x * deltaTime
    pos.y = pos.y + vel.y * deltaTime

    local halfWidth = tex.width / 2
    local halfHeight = tex.height / 2

    if pos.x - halfWidth <= 0 then
        pos.x = halfWidth
        vel.x = -vel.x
    elseif pos.x + halfWidth >= screenSize.x then
        pos.x = screenSize.x - halfWidth
        vel.x = -vel.x
    end

    if pos.y - halfHeight <= 0 then
        pos.y = halfHeight
        vel.y = -vel.y
    elseif pos.y + halfHeight >= screenSize.y then
        pos.y = screenSize.y - halfHeight
        vel.y = -vel.y
    end

    bgColor.x = lerp(bgColor.x, targetColor.x, colorSpeed * deltaTime)
    bgColor.y = lerp(bgColor.y, targetColor.y, colorSpeed * deltaTime)
    bgColor.z = lerp(bgColor.z, targetColor.z, colorSpeed * deltaTime)

    local threshold = 0.01
    if math.abs(bgColor.x - targetColor.x) < threshold and
            math.abs(bgColor.y - targetColor.y) < threshold and
            math.abs(bgColor.z - targetColor.z) < threshold then
        targetColor = Vector3f.new(math.random(), math.random(), math.random())
    end

    local fps = window.getFPS()
    window.setTitle("Hello from Lua! Running at " .. math.floor(fps) .. " FPS")
end

function render()
    draw.clear(bgColor.x, bgColor.y, bgColor.z, 1.0)
    if tex and tex.id and tex.id ~= 0 then
        local size = Vector2f.new(tex.width, tex.height)
        draw.texturedQuad(tex.id, size, pos)
    end
end
