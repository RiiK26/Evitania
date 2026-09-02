# Evitania Online (PC Build)

Evitania is an online Idle RPG with an open world, inspired by classic MMORPGs but without the endless grind. Your character continues to fight, progress, and gather resources even while you're offline, allowing you to focus on what truly matters — rare loot, meaningful upgrades, and challenging boss battles.

# Features Implemented

The cheat is modularized into several feature categories within `src/Features/`:

### Combat
* **God Mode**: Infinite Health and One-Hit Kill logic (`AttackReceiver`)
* **Aura Kill**: Instantly kills all monsters loaded around you (`EnemyNpcController`)

### Progression
* **Exp Multiplier**: Multiply experience gained by x1000 for Combat, Mining, and Woodcutting (`Skill::AddExperience`)

### World
* **Item Magnet**: Automatically and instantly collect all monster drops (`LootItem::BeginLife`)
* **Monster Instant Respawn**: Removes respawn timers and maximizes spawn counts (`EnemySpawnerService`)

### Economy
* **Infinite Gold & Diamonds**: Prevents currencies from decreasing (`CurrencyService`)
* **Infinite Items**: Prevents items from being consumed from inventory (`BaseStorageService`)

### Crafting
* **100% Enhance Item**: Guarantee success when enhancing gear (`EnhanceStationService`)
* **Free Crafting & Smelting**: Craft items and smelt without spending resources (`SpendableVisitor`)
* **Fast Production**: Extremely rapid crafting and smelting speed (`BaseProductionProcessorService`)

### Vendor
* **Discounted Vendor (Free)**: All vendor items cost 0 (`ShopMarketLotDescription`)
* **Increased Vendor Stock**: Buy infinite stock from vendors (`VendorService`)

## Build and Run

The project has been configured to build via CMake and MinGW-w64 for Windows (compatible with Proton/Wine on Linux).

To build the project:
```bash
./scripts/build.sh
```

To run the game and inject the DLL (on Linux/Proton):
```bash
./scripts/run.sh
```

## License
This Project under [MIT License](license)
